#pragma once
#include <types.h>
#include <memory.h>
#include <sync.h>
#include <stack.h>

enum THREAD_STATE
{
	THREAD_STATE_INITIALIZED,
	THREAD_STATE_READY,
	THREAD_STATE_RUNNING,
	THREAD_STATE_BLOCKING,
	THREAD_STATE_TERMINATED

};

struct PROCESS;
class Event;

struct THREAD
{
	int id;
	bool inserted;
	size_t stack;
	size_t kernel_esp;
	THREAD_STATE state;
	THREAD *prev;
	THREAD *next;
	THREAD *proc_next;
	PROCESS *process;
	uint64 sleep_until;
	ADDRESS_SPACE addr_space;
	Event *event;
	uint64 event_until;
	bool event_interruptible;
	bool signaled;
	bool interrupted;
	int sigret_count;
	Stack<THREAD *> signal_threads;
	Event signal_event;
	Event syscall_event;
	uint8 fpu_state[512];
} __attribute__((aligned(16)));
