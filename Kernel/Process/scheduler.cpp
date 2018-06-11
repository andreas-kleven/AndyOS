#include "scheduler.h"
#include "HAL/hal.h"
#include "Memory/memory.h"
#include "string.h"
#include "debug.h"

THREAD* first_thread;
THREAD* last_thread;
THREAD* idle_thread;
THREAD* Scheduler::current_thread;

uint32 id_counter = 0;
uint32 tmp_stack;
IRQ_HANDLER pit_isr;

__declspec(align(16)) uint8 fpu_state[512];

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

THREAD* Scheduler::CreateKernelThread(void* main)
{
	THREAD* thread = (THREAD*)((uint32)(new char[BLOCK_SIZE]) + BLOCK_SIZE - sizeof(THREAD));
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

THREAD* Scheduler::CreateUserThread(void* main, void* stack)
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
	_asm cli
	IDT::SetISR(TASK_SCHEDULE_IRQ, (IRQ_HANDLER)Task_ISR, 0);

	uint32 stack = idle_thread->stack;

	//Start idle thread
	_asm
	{
		mov esp, stack

		pop gs
		pop fs
		pop es
		pop ds
		popad

		iretd
	}
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

void Scheduler::Exit(int exitcode)
{
	RemoveThread(current_thread);
}

void Scheduler::Schedule()
{
	_asm fxsave[fpu_state];
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

		//Waiting
		if (current_thread->state == THREAD_STATE_BLOCKING)
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

	if (current_thread == idle_thread)
		current_thread = current_thread->next;

	//Restore stack
	tmp_stack = current_thread->stack;

	VMem::SwitchDir(current_thread->page_dir);
	TSS::SetStack(KERNEL_SS, current_thread->kernel_esp);
	PIC::InterruptDone(TASK_SCHEDULE_IRQ);

	//Restore fpu state
	if (current_thread->state != THREAD_STATE_INITIALIZED)
	{
		memcpy(fpu_state, current_thread->fpu_state, 512);
		_asm fxrstor[fpu_state];
	}

	current_thread->state = THREAD_STATE_RUNNING;
}

void INTERRUPT Scheduler::Task_ISR()
{
	_asm
	{
		cli

		//Push registers
		pushad

		push ds
		push es
		push fs
		push gs

		//Save esp
		mov[tmp_stack], esp

		//Schedule
		call Schedule

		//Call pit_isr
		push 0
		call pit_isr
		add esp, 4

		//Load esp
		mov esp, [tmp_stack]

		//Load registers
		pop gs
		pop fs
		pop es
		pop ds

		popad

		//Return
		iretd
	}
}

void Scheduler::Idle()
{
	while (1)
	{
		//Debug::Print("Idle");
		_asm pause
	}
}