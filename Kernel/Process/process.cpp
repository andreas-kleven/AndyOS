#include "process.h"
#include "FS/vfs.h"
#include "HAL/hal.h"
#include "Memory/memory.h"
#include "elf.h"
#include "scheduler.h"
#include "string.h"
#include "Lib/debug.h"

int procIdCounter = 0;
PROCESS* first = 0;

PROCESS::PROCESS(PROCESS_FLAGS flags, PAGE_DIR* page_dir)
{
	this->id = ++procIdCounter;
	this->flags = flags;
	this->page_dir = page_dir;
	this->next = 0;
	this->main_thread = 0;

	this->signal_handler = 0;
	this->messages = CircularBuffer<MESSAGE>(PROC_MAX_MESSAGES);
}

PROCESS* ProcessManager::Load(char* path)
{
	PROCESS* proc = ELF::Load(path);

	if (!proc)
		return 0;

	proc->next = first;
	first = proc;

	//Start threads
	THREAD* thread = proc->main_thread;
	while (thread)
	{
		Scheduler::InsertThread(thread);
		thread = thread->procNext;
	}

	return proc;
}

STATUS ProcessManager::Terminate(PROCESS* proc)
{
	THREAD* thread = proc->main_thread;
	while (thread)
	{
		Scheduler::ExitThread(0, thread);
		thread = thread->procNext;
	}

	return STATUS_SUCCESS;
}

STATUS ProcessManager::Kill(PROCESS* proc)
{
	return STATUS_FAILED;
}

THREAD* ProcessManager::CreateThread(PROCESS* proc, void(*entry)())
{
	THREAD* thread = 0;

	//Create thread
	switch (proc->flags)
	{
	case PROCESS_KERNEL:
		thread = Scheduler::CreateKernelThread(entry);
		break;

	case PROCESS_USER:
		VMem::SwitchDir(proc->page_dir);

		uint32 stackPhys = (uint32)PMem::AllocBlocks(2);
		uint8* stack = (uint8*)VMem::UserMapFirstFree(stackPhys, PTE_PRESENT | PTE_WRITABLE | PTE_USER, 2);

		thread = Scheduler::CreateUserThread(entry, stack + BLOCK_SIZE);
		break;
	}

	if (!thread)
		return 0;

	thread->page_dir = proc->page_dir;
	thread->process = proc;

	//Insert into thread list
	if (proc->main_thread == 0)
	{
		proc->main_thread = thread;
		thread->procNext = 0;
	}
	else
	{
		thread->procNext = proc->main_thread->procNext;
		proc->main_thread->procNext = thread;
	}

	return thread;
}

STATUS ProcessManager::RemoveThread(THREAD* thread)
{
	return STATUS_FAILED;
}


PROCESS* ProcessManager::GetProcess(int id)
{
	PROCESS* proc = first;

	while (proc)
	{
		if (proc->id == id)
			return proc;

		proc = proc->next;
	}

	return 0;
}

PROCESS* ProcessManager::GetFirst()
{
	return first;
}