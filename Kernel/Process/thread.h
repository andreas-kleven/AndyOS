#pragma once
#include "definitions.h"
#include "HAL/idt.h"

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
	REGS regs;
	Thread* next;

	Thread();
};