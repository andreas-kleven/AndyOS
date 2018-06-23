#pragma once
#include "Memory/memory.h"
#include "thread.h"
#include "Lib/circbuf.h"

#define PROC_MAX_MESSAGES 32

typedef void SIGNAL_HANDLER(int);

enum PROCESS_FLAGS
{
	PROCESS_USER,
	PROCESS_KERNEL
};

enum MESSAGE_TYPE
{
	MESSAGE_TYPE_SIGNAL,
	MESSAGE_TYPE_2
};

struct MESSAGE
{
	MESSAGE_TYPE type;
	int signo;
	char* data;
};

struct PROCESS
{
	uint16 id;
	PROCESS_FLAGS flags;
	PAGE_DIR* page_dir;
	THREAD* main_thread;
	PROCESS* next;

	SIGNAL_HANDLER* signal_handler;
	CircularBuffer<MESSAGE> messages;

	PROCESS(PROCESS_FLAGS flags, PAGE_DIR* page_dir);
};

class ProcessManager
{
public:
	static PROCESS* Load(char* path);
	static STATUS Terminate(PROCESS* proc);
	static STATUS Kill(PROCESS* proc);
	static THREAD* CreateThread(PROCESS* proc, void(*entry)());
	static STATUS RemoveThread(THREAD* thread);

	static PROCESS* GetProcess(int id);
};

