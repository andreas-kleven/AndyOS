#include "task.h"
#include "hal.h"
#include "debug.h"

THREAD* Task::current_thread;

STATUS Task::Init()
{
	current_thread = CreateThread(Idle);
	current_thread->next = current_thread;


	int stack = current_thread->esp;

	_asm
	{
		mov esp, stack
		//pop gs
		//pop fs
		//pop es
		//pop ds
		//popad
		mov eax, Idle

		//push 0x23
		//push esp
		//push 0x202
		//push 0x10
		//push eax
		//iretd
	}

	return STATUS_SUCCESS;
}

THREAD* Task::CreateThread(void* main)
{
	THREAD* thread = (THREAD*)(new uint8[0x1000]);
	//TASK_REGS* regs = (TASK_REGS*)((uint8*)thread + sizeof(THREAD) - sizeof(TASK_REGS));

	thread->esp = (uint32)thread;
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
	//regs->user_ss = 0x2B;

	//regs->cs = 8;
	//regs->ds = 0x10;
	//regs->es = 0x10;
	//regs->fs = 0x10;
	//regs->gs = 0x10;

	return thread;
}

void Task::Schedule()
{
	current_thread = current_thread->next;

}

void INTERRUPT Task::Task_ISR()
{
	_asm pushad

	PIC::InterruptDone(12);
	//Debug::Print("done");

	int* as;
	_asm mov eax, esp
	_asm mov as, eax
	Debug::Dump(as, 32);
	while (1);

	_asm popad

	_asm
	{
		cli
		pushad

		//push ds
		//push es
		//push fs
		//push gs

		//mov ax, 0x10
		//mov ds, ax
		//mov es, ax
		//mov fs, ax
		//mov gs, ax

		//Save esp
		mov eax, [current_thread]
		mov[eax], esp

		//Schedule
		call Schedule

		mov eax, [current_thread]

		//Set page directory
		//mov ebx, [eax + 12]
		//mov cr3, ebx

		//Set esp
		mov esp, [eax]

		//Set kernel stack
		//push[eax + 8]
		//push[eax + 4]
		////call tss_set_stack
		//add esp, 8

		pop gs
		pop fs
		pop es
		pop ds

		popad
		iretd
		//jmp pit_isr

	}
}

void _declspec (naked) Task::Idle()
{
	while (1)
	{
		Debug::Print("wurks");
	}
}
