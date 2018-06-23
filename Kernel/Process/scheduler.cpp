#include "scheduler.h"
#include "HAL/hal.h"
#include "Memory/memory.h"
#include "string.h"
#include "Lib/debug.h"

THREAD* first_thread;
THREAD* last_thread;
THREAD* idle_thread;
THREAD* Scheduler::current_thread;

uint32 id_counter = 0;
uint32 tmp_stack;
IRQ_HANDLER pit_isr;

uint8 __attribute__((aligned(16))) fpu_state[512];

STATUS Scheduler::Init()
{
	first_thread = 0;
	last_thread = 0;
	current_thread = 0;

	idle_thread = CreateKernelThread(Idle);
	InsertThread(idle_thread);

	pit_isr = IDT::GetHandler(TASK_SCHEDULE_IRQ);
	return STATUS_SUCCESS;
}

THREAD* Scheduler::CreateKernelThread(void(*main)())
{
	THREAD* thread = (THREAD*)((uint32)(new char[BLOCK_SIZE]) + BLOCK_SIZE - sizeof(THREAD));
	thread->regs = (REGS*)((uint32)thread - sizeof(REGS));
	thread->regs->ebp = 0;
	thread->regs->esp = 0;
	thread->regs->edi = 0;
	thread->regs->esi = 0;
	thread->regs->edx = 0;
	thread->regs->ecx = 0;
	thread->regs->ebx = 0;
	thread->regs->eax = 0;
	thread->regs->user_stack = 0;
	thread->regs->user_ss = 0;
	thread->kernel_esp = 0;
	thread->page_dir = 0;
	thread->next = 0;
	thread->procNext = 0;

	thread->id = ++id_counter;
	thread->state = THREAD_STATE_INITIALIZED;
	thread->stack = (uint32)(thread - 1) - sizeof(REGS);
	thread->regs = (REGS*)thread->stack;
	thread->regs->esp = (uint32)&thread->regs;
	thread->regs->eip = (uint32)main;
	thread->regs->eflags = 0x200;
	thread->regs->cs = KERNEL_CS;
	thread->regs->ds = KERNEL_SS;
	thread->regs->es = KERNEL_SS;
	thread->regs->fs = KERNEL_SS;
	thread->regs->gs = KERNEL_SS;

	thread->page_dir = VMem::GetCurrentDir();
	thread->fpu_state = new uint8[512];

	return thread;
}

THREAD* Scheduler::CreateUserThread(void(*main)(), void* stack)
{
	THREAD* thread = CreateKernelThread(main);
	thread->kernel_esp = thread->stack;

	thread->regs->cs = USER_CS;
	thread->regs->ds = USER_SS;
	thread->regs->es = USER_SS;
	thread->regs->fs = USER_SS;
	thread->regs->gs = USER_SS;
	thread->regs->user_stack = (uint32)stack;
	thread->regs->user_ss = USER_SS;

	return thread;
}

void Scheduler::InsertThread(THREAD* thread)
{
	if (first_thread)
	{
		thread->next = first_thread;
		last_thread->next = thread;
		last_thread = thread;
	}
	else
	{
		thread->next = thread;
		first_thread = thread;
		last_thread = thread;
		current_thread = thread;
	}
}

void Scheduler::StartThreading()
{
	asm volatile("cli");
	IDT::SetISR(TASK_SCHEDULE_IRQ, Task_ISR, 0);

	asm volatile(
		"mov %0, %%esp\n"
		"pop %%gs\n"
		"pop %%fs\n"
		"pop %%es\n"
		"pop %%ds\n"
		"popa\n"
		"iret" :: "r" (idle_thread->stack));
}

void Scheduler::RemoveThread(THREAD* thread)
{
	if (!thread)
		return;

	THREAD* t = first_thread;
	while (t)
	{
		THREAD* next = t->next;

		if (next == thread)
		{
			t->next = next->next;

			if (thread == current_thread)
				current_thread = t;

			delete thread;
			return;
		}

		t = next;
	}
}

void Scheduler::ExitThread(int code, THREAD* thread)
{
	RemoveThread(thread);
}

void Scheduler::SleepThread(uint32 until, THREAD* thread)
{
	if (thread->state != THREAD_STATE_TERMINATED)
	{
		thread->state = THREAD_STATE_SLEEPING;
		thread->sleep_until = until;

		//Switch thread
		if (thread == current_thread)
			asm volatile("int %0" :: "N" (TASK_SCHEDULE_IRQ));
	}
}

void Scheduler::BlockThread(THREAD* thread)
{
	if (thread->state != THREAD_STATE_TERMINATED)
	{
		thread->state = THREAD_STATE_BLOCKING;

		//Switch thread
		if (thread == current_thread)
			asm volatile("int %0" :: "N" (TASK_SCHEDULE_IRQ));
	}
}

void Scheduler::AwakeThread(THREAD* thread)
{
	if (thread->state != THREAD_STATE_TERMINATED)
	{
		if (PIT::ticks >= thread->sleep_until)
		{
			thread->state = THREAD_STATE_READY;
		}
		else
		{
			thread->state = THREAD_STATE_SLEEPING;
		}
	}
}

void Scheduler::Schedule()
{
	//Call original isr
	pit_isr(0);

	asm volatile("fxsave (%0)" :: "m" (fpu_state));
	memcpy(current_thread->fpu_state, fpu_state, 512);

	//Save stack
	current_thread->stack = tmp_stack;
	current_thread->regs = (REGS*)tmp_stack;

	if (current_thread->state == THREAD_STATE_RUNNING)
		current_thread->state = THREAD_STATE_READY;

	//Schedule
	THREAD* first = current_thread;

	while (current_thread->next != first)
	{
		current_thread = current_thread->next;

		if (current_thread == idle_thread)
			current_thread = current_thread->next;

		//Waiting
		if (current_thread->state == THREAD_STATE_SLEEPING)
		{
			if (PIT::ticks >= current_thread->sleep_until)
			{
				current_thread->sleep_until = 0;
				current_thread->state = THREAD_STATE_READY;
			}
		}

		if (current_thread->state == THREAD_STATE_READY || current_thread->state == THREAD_STATE_INITIALIZED)
			break;
	}

	//Restore stack
	tmp_stack = current_thread->stack;

	VMem::SwitchDir(current_thread->page_dir);
	TSS::SetStack(KERNEL_SS, current_thread->kernel_esp);
	PIC::InterruptDone(TASK_SCHEDULE_IRQ);

	//Restore fpu state
	if (current_thread->state != THREAD_STATE_INITIALIZED)
	{
		memcpy(fpu_state, current_thread->fpu_state, 512);
		asm volatile("fxrstor (%0)" : "=m" (fpu_state));
	}

	current_thread->state = THREAD_STATE_RUNNING;
}

void INTERRUPT Scheduler::Task_ISR()
{
	asm volatile(
		"cli\n"

		//Save registers
		"pusha\n"
		"push %%ds\n"
		"push %%es\n"
		"push %%fs\n"
		"push %%gs\n"

		"mov %%esp, %0\n"

		//Schedule
		"call %P1\n"

		//Load registers
		"mov %2, %%esp\n"

		"pop %%gs\n"
		"pop %%fs\n"
		"pop %%es\n"
		"pop %%ds\n"
		"popa\n"

		"iret"
		: "=m" (tmp_stack) : "i" (&Schedule), "m" (tmp_stack));
}

void Scheduler::Idle()
{
	while (1)
	{
		//Debug::Print("Idle");
		asm volatile("pause");
	}
}