#pragma once
#include "definitions.h"
#include "HAL/idt.h"

#define TASK_SCHEDULE_IRQ 32

struct THREAD
{
	uint32 stack;
	REGS regs;
	__declspec(align(16)) uint8 fpu_state[512];
	THREAD* next;
	uint32 id;

	THREAD();
};