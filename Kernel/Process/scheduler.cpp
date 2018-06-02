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

	idle_thread = CreateThread(Idle);
	InsertThread(idle_thread);

	pit_isr = IDT::GetHandler(TASK_SCHEDULE_IRQ);
	return STATUS_SUCCESS;
}

Thread* Scheduler::CreateThread(void* main)
{
	Thread* thread = new Thread;
	//TASK_REGS* regs = (TASK_REGS*)((uint8*)thread + sizeof(THREAD) - sizeof(TASK_REGS));
	thread->regs.esp = uint32(new char[PAGE_SIZE] + 0x1000);

	thread->regs.eflags = 0x202;
	thread->regs.eip = (uint32)main;
	thread->regs.ebp = 0;
	thread->regs.esp = 0;
	thread->regs.edi = 0;
	thread->regs.esi = 0;
	thread->regs.edx = 0;
	thread->regs.ecx = 0;
	thread->regs.ebx = 0;
	thread->regs.eax = 0;

	//uint16* c = (uint16*)&regs->cs;
	//_asm mov eax, c
	//_asm mov [eax], cs
	//
	//Debug::Print("%x\n", *c);
	thread->regs.cs = KERNEL_CS;

	thread->regs.user_stack = (uint32)thread->regs.esp;
	thread->regs.user_ss = KERNEL_SS;

	thread->regs.ds = 0x10;
	thread->regs.es = 0x10;
	thread->regs.fs = 0x10;
	thread->regs.gs = 0x10;

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
		mov ax, KERNEL_SS
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