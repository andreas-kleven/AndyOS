#include "task.h"
#include "hal.h"
#include "memory.h"
#include "drawing.h"
#include "debug.h"

Thread* idle_thread;
Thread* current_thread;
Thread* first_thread;
Thread* last_thread;

STATUS Task::Init()
{
	first_thread = 0;
	last_thread = 0;
	current_thread = 0;

	idle_thread = CreateThread(Idle);
	InsertThread(idle_thread);

	return STATUS_SUCCESS;
}

Thread* Task::CreateThread(void* main)
{
	Thread* thread = new Thread;
	//TASK_REGS* regs = (TASK_REGS*)((uint8*)thread + sizeof(THREAD) - sizeof(TASK_REGS));
	void* esp = (char*)Memory::AllocBlocks(1) + 0x1000 - sizeof(TASK_REGS);

	thread->esp = (uint32)esp;
	TASK_REGS* regs = (TASK_REGS*)thread->esp;

	regs->flags = 0x202;
	regs->eip = (uint32)main;
	regs->ebp = 0;
	regs->esp = 0;
	regs->edi = 0;
	regs->esi = 0;
	regs->edx = 0;
	regs->ecx = 0;
	regs->ebx = 0;
	regs->eax = 0;

	//uint16* c = (uint16*)&regs->cs;
	//_asm mov eax, c
	//_asm mov [eax], cs
	//
	//Debug::Print("%x\n", *c);
	regs->cs = 0x10;

	regs->user_stack = (uint32)thread->esp;
	regs->user_ss = 0x23;

	regs->ds = 0x18;
	regs->es = 0x18;
	regs->fs = 0x18;
	regs->gs = 0x18;

	return thread;
}

void Task::InsertThread(Thread* thread)
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

void Task::StartThreading()
{
	_asm cli
	IDT::SetISR(32, Task_ISR);

	//Start idle thread
	uint32 stack = idle_thread->esp;
	_asm
	{
		mov esp, stack

		pop gs
		pop fs
		pop es
		pop ds
		popad

		sti
		iretd
	}
}

void Task::Schedule()
{
	current_thread = current_thread->next;

	if (current_thread == idle_thread)
		current_thread = current_thread->next;
}

void INTERRUPT Task::Task_ISR()
{
	_asm pushad
	PIC::InterruptDone(12);
	_asm popad

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

		//Set esp
		mov eax, [current_thread]
		mov esp, [eax]

		//Pop registers
		pop gs
		pop fs
		pop es
		pop ds

		popad

		//Return
		sti
		iretd
	}
}

void _declspec (naked) Task::Idle()
{
	while (1)
	{
		Debug::Print("Idle");
		_asm pause
	}
}