#pragma once
#include "types.h"
#include "Memory/memory.h"

enum THREAD_STATE
{
	THREAD_STATE_INITIALIZED,
	THREAD_STATE_READY,
	THREAD_STATE_RUNNING,
	THREAD_STATE_BLOCKING,
	THREAD_STATE_TERMINATED

};

struct PROCESS;

struct THREAD
{
	int id;
	bool inserted;
	size_t stack;
	size_t kernel_esp;
	THREAD_STATE state;
	THREAD* prev;
	THREAD* next;
	THREAD* proc_next;
	PROCESS* process;
	void* fpu_state;
	uint64 sleep_until;
	ADDRESS_SPACE addr_space;
};