#include "scheduler.h"
#include "HAL/hal.h"
#include "Memory/memory.h"
#include "debug.h"

Thread* idle_thread;
Thread* current_thread;
Thread* first_thread;
Thread* last_thread;

IRQ_HANDLER pit_isr;

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

Thread* Scheduler::CreateKernelThread(void* main)
{
	Thread* thread = new Thread;
	thread->regs.esp = uint32(new char[PAGE_SIZE] + 0x1000);
	thread->regs.eip = (uint32)main;

	thread->regs.eflags = 0x200;

	thread->regs.cs = KERNEL_CS;
	thread->regs.ds = KERNEL_SS;
	thread->regs.es = KERNEL_SS;
	thread->regs.fs = KERNEL_SS;
	thread->regs.gs = KERNEL_SS;

	thread->regs.user_stack = (uint32)thread->regs.esp;
	thread->regs.user_ss = KERNEL_SS;

	return thread;
}

Thread* Scheduler::CreateUserThread(void* main, void* stack)
{
	Thread* thread = new Thread;
	thread->regs.esp = uint32(new char[PAGE_SIZE] + 0x1000);
	thread->regs.eip = (uint32)main;

	thread->regs.eflags = 0x200;

	thread->regs.cs = KERNEL_CS;
	thread->regs.ds = KERNEL_SS;
	thread->regs.es = KERNEL_SS;
	thread->regs.fs = KERNEL_SS;
	thread->regs.gs = KERNEL_SS;

	thread->regs.user_stack = (uint32)stack;
	thread->regs.user_ss = USER_SS;

	return thread;
}

void Scheduler::InsertThread(Thread* thread)
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
	IDT::InstallIRQ(TASK_SCHEDULE_IRQ, (IRQ_HANDLER)Task_ISR);

	uint32 stack = idle_thread->regs.esp;
	uint32 flags = idle_thread->regs.eflags;
	uint32 entry = idle_thread->regs.eip;

	//Start idle thread
	_asm
	{
		mov ax, KERNEL_DS
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax

		push KERNEL_SS
		push[stack]

		push flags

		push KERNEL_CS
		push entry
		iretd
	}
}

void Scheduler::RemoveThread(Thread* thread)
{
	_asm cli

	if (!thread)
		return;

	Thread* t = first_thread;
	while (t)
	{
		Thread* next = t->next;

		if (next == thread)
		{
			t->next = next->next;

			if (thread == current_thread)
				current_thread = t;

			delete thread;
			_asm sti
			return;
		}

		t = next;
	}

	_asm sti
}

void Scheduler::Exit(int exitcode)
{
	RemoveThread(current_thread);
}

void Scheduler::Schedule()
{
	current_thread = current_thread->next;

	if (current_thread == idle_thread)
		current_thread = current_thread->next;
}

void Scheduler::Task_ISR(REGS* regs)
{
	current_thread->regs = *regs;
	Schedule();
	*regs = current_thread->regs;
	TSS::SetStack(KERNEL_SS, current_thread->regs.esp);
	pit_isr(regs);
}

void _declspec (naked) Scheduler::Idle()
{
	while (1)
	{
		//Debug::Print("Idle");
		_asm pause
	}
}