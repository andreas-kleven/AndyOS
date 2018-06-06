#pragma once
#include "definitions.h"
#include "Memory/memory.h"
#include "HAL/idt.h"

#define TASK_SCHEDULE_IRQ 32

enum THREAD_STATE
{
	THREAD_STATE_INITIALIZED,
	THREAD_STATE_READY,
	THREAD_STATE_RUNNING,
	THREAD_STATE_STANDBY,
	THREAD_STATE_TERMINATED

};

struct THREAD
{
	uint32 stack;
	REGS* regs;
	uint32 kernel_esp;
	PAGE_DIR* page_dir;
	uint32 id;
	THREAD_STATE state;
	THREAD* next;
	THREAD* procNext;
	uint8* fpu_state;
};