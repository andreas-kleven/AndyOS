#pragma once
#include "definitions.h"
#include "Memory/memory.h"
#include "HAL/idt.h"

#define TASK_SCHEDULE_IRQ 32

struct THREAD
{
	uint32 stack;
	REGS* regs;
	uint32 kernel_esp;
	PAGE_DIR* page_dir;
	uint32 id;
	THREAD* next;
	THREAD* procNext;

	__declspec(align(16)) uint8 fpu_state[512];

	THREAD();
};