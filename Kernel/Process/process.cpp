#include "process.h"
#include "FS/vfs.h"
#include "hal.h"
#include "Memory/memory.h"
#include "elf.h"
#include "scheduler.h"
#include "Kernel/task.h"
#include "string.h"
#include "Lib/debug.h"

PROCESS::PROCESS(PROCESS_FLAGS flags, ADDRESS_SPACE addr_space)
{
	this->flags = flags;
	this->addr_space = addr_space;
	this->next = 0;
	this->main_thread = 0;
	this->stack_ptr = USER_END;
	this->heap_end = 0;

	memset(this->file_table, 0, sizeof(this->file_table));

	this->signal_handler = 0;
	this->messages = CircularBuffer<MESSAGE>(PROC_MAX_MESSAGES);
}

namespace ProcessManager
{
	PROCESS *first = 0;

	pid_t pid_counter = 1;
	pid_t AssignPid(PROCESS *proc)
	{
		if (!proc)
			return 0;

		proc->id = pid_counter++;
		return proc->id;
	}

	PROCESS *AddProcess(PROCESS *proc)
	{
		if (!proc)
			return 0;

		if (proc->id <= 0)
			AssignPid(proc);

		proc->next = first;
		first = proc;

		//Start threads
		THREAD *thread = proc->main_thread;
		while (thread)
		{
			Scheduler::InsertThread(thread);
			thread = thread->procNext;
		}

		return proc;
	}

	STATUS Terminate(PROCESS *proc)
	{
		FreeAllMemory(proc);
		CloseFiles(proc);
		StopThreads(proc);
		Task::Switch(); //execution stops here
		return STATUS_SUCCESS;
	}

	STATUS Kill(PROCESS *proc)
	{
		return STATUS_FAILED;
	}

	THREAD *CreateThread(PROCESS *proc, void (*entry)())
	{
		THREAD *thread = 0;

		//Create thread
		switch (proc->flags)
		{
		case PROCESS_KERNEL:
			thread = Task::CreateKernelThread(entry);
			break;

		case PROCESS_USER:
			VMem::SwapAddressSpace(proc->addr_space);

			int flags = PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
			int blocks = 2; // TODO: Hack

			void *virt = (void *)(proc->stack_ptr - blocks * BLOCK_SIZE);
			void *phys = PMem::AllocBlocks(blocks);
			VMem::MapPages(virt, phys, blocks, flags);

			thread = Task::CreateUserThread(entry, (void *)(proc->stack_ptr - 0x1000));
			proc->stack_ptr = (size_t)virt;
			break;
		}

		if (AddThread(proc, thread))
			return thread;

		return 0;
	}

	bool AddThread(PROCESS *proc, THREAD *thread)
	{
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

		return true;
	}

	bool RemoveThread(THREAD *thread)
	{
		return false;
	}

	bool StopThreads(PROCESS *proc)
	{
		THREAD *thread = proc->main_thread;
		while (thread)
		{
			Scheduler::ExitThread(0, thread, false);
			thread = thread->procNext;
		}

		return true;
	}

	bool FreeAllMemory(PROCESS *proc)
	{
		size_t count = (USER_END - USER_BASE) / PAGE_SIZE;
		VMem::FreePages((void *)USER_BASE, count);
		return true;
	}

	bool CloseFiles(PROCESS *proc)
	{
		for (int fd = 0; fd < FILE_TABLE_SIZE; fd++)
		{
			FILE *file = proc->file_table[fd];

			if (file)
			{
				if (VFS::Close(fd) == -1)
					return false;
			}
		}

		return true;
	}

	void *AdjustHeap(PROCESS *proc, int increment)
	{
		size_t prev_end = proc->heap_end;
		size_t next_end = prev_end + increment;
		int cur_block = (prev_end - 1) / BLOCK_SIZE + 1;
		int next_block = (next_end - 1) / BLOCK_SIZE + 1;
		int blocks = next_block - cur_block;

		if (next_end > HEAP_END)
			return 0;

		if (blocks > 0)
		{
			int flags = PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
			size_t virt = cur_block * BLOCK_SIZE;
			void *phys = PMem::AllocBlocks(blocks);
			VMem::MapPages((void *)virt, phys, blocks, flags);
		}
		else if (blocks < 0)
		{
			void* virt = (void*)(prev_end - blocks * BLOCK_SIZE);
			VMem::FreePages(virt, blocks);
		}

		proc->heap_end = next_end;
		return (void *)prev_end;
	}

	PROCESS *GetCurrent()
	{
		return Scheduler::CurrentThread()->process;
	}

	PROCESS *GetProcess(pid_t id)
	{
		PROCESS *proc = first;

		while (proc)
		{
			if (proc->id == id)
				return proc;

			proc = proc->next;
		}

		return 0;
	}

	PROCESS *GetFirst()
	{
		return first;
	}
} // namespace ProcessManager