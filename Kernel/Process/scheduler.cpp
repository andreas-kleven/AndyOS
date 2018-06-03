#include "scheduler.h"
#include "HAL/hal.h"
#include "Memory/memory.h"
#include "debug.h"

Thread* idle_thread;
Thread* current_thread;
Thread* first_thread;
Thread* last_thread;

uint32 id_counter = 0;
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
	Thread* thread = (Thread*)((uint32)(new char[BLOCK_SIZE]) + BLOCK_SIZE - sizeof(Thread));
	thread->id = ++id_counter;

	thread->stack = (uint32)&thread->regs;
	thread->regs.esp = (uint32)&thread->regs;
	thread->regs.eip = (uint32)main;
	thread->regs.eflags = 0x200;
	thread->regs.cs = KERNEL_CS;
	thread->regs.ds = KERNEL_SS;
	thread->regs.es = KERNEL_SS;
	thread->regs.fs = KERNEL_SS;
	thread->regs.gs = KERNEL_SS;

	return thread;
}

Thread* Scheduler::CreateUserThread(void* main, void* stack)
{
	Thread* thread = CreateKernelThread(main);
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

	Debug::color = 0xFFFF0000;
	Debug::Print("\t%i\t%ux\t%ux\n", thread->id, thread->regs.cs, thread->regs.esp);
	Debug::color = 0xFF00FF00;
}

void Scheduler::StartThreading()
{
	_asm cli
	IDT::SetISR(TASK_SCHEDULE_IRQ, (IRQ_HANDLER)Task_ISR, 0);

	uint32 stack = idle_thread->regs.esp;

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
	Debug::Print("\t%i\t%ux\t%ux\n", current_thread->id, current_thread->regs.cs, current_thread->regs.esp);
	Debug::Dump(current_thread, 4 + sizeof(REGS));
	current_thread = current_thread->next;

	if (current_thread == idle_thread)
		current_thread = current_thread->next;

	Debug::Print("\t%i\t%ux\t%ux\n", current_thread->id, current_thread->regs.cs, current_thread->regs.esp);
	Debug::Dump(current_thread, 4 + sizeof(REGS));

	PIC::InterruptDone(TASK_SCHEDULE_IRQ);
}

uint32 fpu_state;

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
		mov eax, [current_thread]
		mov[eax], esp

		//Schedule
		call Schedule

		//Call pit_isr
		//push 0
		//call pit_isr
		//add esp, 4

		//Load esp
		mov eax, [current_thread]
		mov esp, [eax]

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

void _declspec (naked) Scheduler::Idle()
{
	while (1)
	{
		//Debug::Print("Idle");
		_asm pause
	}
}