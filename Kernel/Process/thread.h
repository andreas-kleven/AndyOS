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
	THREAD_STATE_SLEEPING,
	THREAD_STATE_TERMINATED

};

struct PROCESS;

struct THREAD
{
	uint32 stack;
	REGS* regs;
	uint32 kernel_esp;
	ADDRESS_SPACE addr_space;
	uint32 id;
	THREAD_STATE state;
	THREAD* next;
	THREAD* procNext;
	PROCESS* process;
	uint8* fpu_state;
	uint32 sleep_until;
};