#pragma once
#include "Memory/memory.h"
#include "thread.h"
#include "FS/file.h"
#include "Lib/circbuf.h"

#define PROC_MAX_MESSAGES 32
#define FILE_TABLE_SIZE 256

typedef void SIGNAL_HANDLER(int signo);
typedef void MESSAGE_HANDLER(int id, int type, char* buf, int size);

enum PROCESS_FLAGS
{
	PROCESS_USER,
	PROCESS_KERNEL
};

enum MESSAGE_TYPE
{
	MESSAGE_TYPE_SIGNAL,
	MESSAGE_TYPE_MESSAGE,
	MESSAGE_TYPE_RESPONSE
};

struct MESSAGE
{
	MESSAGE_TYPE type;
	int id;
	int src_proc;
	int param;
	int size;
	char* data;

	MESSAGE()
	{
		id = 0;
		src_proc = 0;
		param = 0;
		size = 0;
		data = 0;
	}

	MESSAGE(MESSAGE_TYPE type, int id, int src_proc, int param, int size, char* data)
	{
		this->type = type;
		this->id = id;
		this->src_proc = src_proc;
		this->param = param;
		this->size = size;
		this->data = data;
	}

	MESSAGE(MESSAGE_TYPE type, int id, int src_proc, int param, int size)
	{
		this->type = type;
		this->id = id;
		this->src_proc = src_proc;
		this->param = param;
		this->size = size;
		this->data = new char[size];
	}
};

struct PROCESS
{
	pid_t id;
	PROCESS_FLAGS flags;
	ADDRESS_SPACE addr_space;
	THREAD* main_thread;
	PROCESS* next;

	FILE* file_table[FILE_TABLE_SIZE];

	SIGNAL_HANDLER* signal_handler;
	MESSAGE_HANDLER* message_handler;
	CircularBuffer<MESSAGE> messages;

	PROCESS(PROCESS_FLAGS flags, ADDRESS_SPACE addr_space);
};

namespace ProcessManager
{
	pid_t AssignPid(PROCESS* proc);

	PROCESS* AddProcess(PROCESS* proc);
	STATUS Terminate(PROCESS* proc);
	STATUS Kill(PROCESS* proc);
	THREAD* CreateThread(PROCESS* proc, void(*entry)());
	bool AddThread(PROCESS* proc, THREAD* thread);
	bool RemoveThread(THREAD* thread);
	bool StopThreads(PROCESS* proc, bool auto_switch);
	bool FreeMemory(PROCESS* proc);

	PROCESS* Exec(const char* path);
	bool Exec(PROCESS* proc, char const *path, char const *argv[], char const *envp[]);
	
	PROCESS* Fork(PROCESS* proc);

	PROCESS* GetCurrent();
	PROCESS* GetProcess(pid_t id);
	PROCESS* GetFirst();
};

