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
	THREAD_STATE_BLOCKING,
	THREAD_STATE_TERMINATED

};

struct THREAD
{
	uint32 stack;
	REGS* regs = 0;
	uint32 kernel_esp = 0;
	PAGE_DIR* page_dir = 0;
	uint32 id = 0;
	THREAD_STATE state;
	THREAD* next = 0;
	THREAD* procNext = 0;
	uint8* fpu_state = 0;
	uint32 sleep_until = 0;

	void Sleep(uint32 until);
};