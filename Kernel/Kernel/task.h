#pragma once
#include "definitions.h"

#define TASK_SCHEDULE_IRQ 32

struct TASK_REGS
{
	uint32 gs;
	uint32 fs;
	uint32 es;
	uint32 ds;

	uint32 eax;
	uint32 ebx;
	uint32 ecx;
	uint32 edx;
	uint32 esi;
	uint32 edi;
	uint32 esp;
	uint32 ebp;

	uint32 eip;
	uint32 cs;
	uint32 flags;

	uint32 user_stack;
	uint32 user_ss;
};

class Thread
{
public:
	//TASK_REGS regs;
	uint32 esp;
	Thread* next;
};

static class Task
{
public:
	static STATUS Init();

	static Thread* CreateThread(void* main);
	static void InsertThread(Thread* thread);

	static void StartThreading();

private:
	static void Schedule();
	static void Task_ISR();

	static void Idle();
};