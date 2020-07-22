#pragma once
#include "Memory/memory.h"
#include "thread.h"
#include "sync.h"
#include "filetable.h"
#include "FS/file.h"
#include "Lib/types.h"
#include "Lib/circbuf.h"

#define PROC_MAX_MESSAGES 32
#define FILE_TABLE_SIZE 256
#define SIGNAL_TABLE_SIZE 32

#define SIG_DFL ((sig_t)0)
#define SIG_IGN ((sig_t)1)
#define SIG_ERR ((sig_t)-1)

#define SIGKILL 9
#define SIGSTOP 17

typedef void (*sig_t)(int signo);
typedef void MESSAGE_HANDLER(int id, int type, char *buf, int size);

enum PROCESS_FLAGS
{
	PROCESS_USER,
	PROCESS_KERNEL
};

enum MESSAGE_TYPE
{
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
	char *data;

	MESSAGE()
	{
		id = 0;
		src_proc = 0;
		param = 0;
		size = 0;
		data = 0;
	}

	MESSAGE(MESSAGE_TYPE type, int id, int src_proc, int param, int size, char *data)
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
	PROCESS *next;

	pid_t id;
	PROCESS_FLAGS flags;
	ADDRESS_SPACE addr_space;
	THREAD *main_thread;
	size_t stack_ptr;
	size_t heap_start;
	size_t heap_end;

	Filetable filetable;
	sig_t signal_table[SIGNAL_TABLE_SIZE];
	Mutex signal_mutex;

	MESSAGE_HANDLER *message_handler;
	CircularBuffer<MESSAGE> messages;

	PROCESS(PROCESS_FLAGS flags, ADDRESS_SPACE addr_space);
};

namespace ProcessManager
{
	pid_t AssignPid(PROCESS *proc);

	PROCESS *AddProcess(PROCESS *proc);
	STATUS Terminate(PROCESS *proc);
	THREAD *CreateThread(PROCESS *proc, void (*entry)());
	bool AddThread(PROCESS *proc, THREAD *thread);
	bool RemoveThread(THREAD *thread);
	bool StopThreads(PROCESS *proc);
	bool FreeAllMemory(PROCESS *proc);
	bool CloseFiles(PROCESS *proc);
	void *AdjustHeap(PROCESS *proc, int increment);

	PROCESS *Fork(PROCESS *proc);

	PROCESS *GetProcess(pid_t id);
	PROCESS *GetFirst();

	PROCESS *Exec(const char *path);
	int Exec(PROCESS *proc, char const *path, char const *argv[], char const *envp[]);

	int SetSignalHandler(PROCESS *proc, int signo, sig_t handler);
	int HandleSignal(PROCESS *proc, int signo);
}; // namespace ProcessManager
