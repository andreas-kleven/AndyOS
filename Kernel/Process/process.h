#pragma once
#include "Memory/memory.h"
#include "thread.h"

struct PROCESS
{
	uint16 id;
	PAGE_DIR* page_dir;
	Thread* thread;
	PROCESS* next;
};

static class Process
{
public:
	static void Create(char* filename);
};

