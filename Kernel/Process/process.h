#pragma once
#include "Memory/memory.h"
#include "thread.h"
#include "Lib/circbuf.h"

#define PROC_MAX_MESSAGES 32

typedef void SIGNAL_HANDLER(int signo);
typedef void MESSAGE_HANDLER(int type, char* buf, int size);

enum PROCESS_FLAGS
{
	PROCESS_USER,
	PROCESS_KERNEL
};

enum MESSAGE_TYPE
{
	MESSAGE_TYPE_SIGNAL,
	MESSAGE_TYPE_MESSAGE
};

struct MESSAGE
{
	MESSAGE_TYPE type;
	int param;
	int size;
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
	MESSAGE_HANDLER* message_handler;
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

