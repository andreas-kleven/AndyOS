#pragma once
#include "Memory/memory.h"
#include "thread.h"

enum PROCESS_FLAGS
{
	PROCESS_USER,
	PROCESS_KERNEL
};

struct PROCESS_INFO
{
	uint16 id;
	PROCESS_FLAGS flags;
	PAGE_DIR* page_dir;
	THREAD* main_thread;
	PROCESS_INFO* next;

	PROCESS_INFO(PROCESS_FLAGS flags, PAGE_DIR* page_dir);
};

class Process
{
public:
	static PROCESS_INFO* Create(char* filename);
	static STATUS Terminate(PROCESS_INFO* proc);
	static STATUS Kill(PROCESS_INFO* proc);
	static THREAD* CreateThread(PROCESS_INFO* proc, void(*main)());
	static STATUS RemoveThread(THREAD* thread);
};

