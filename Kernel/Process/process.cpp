#include <Process/process.h>
#include <FS/vfs.h>
#include <hal.h>
#include <memory.h>
#include <Process/elf.h>
#include <Process/scheduler.h>
#include <Process/dispatcher.h>
#include <Kernel/task.h>
#include <string.h>
#include <debug.h>

PROCESS::PROCESS(PROCESS_FLAGS flags, ADDRESS_SPACE addr_space)
{
	this->flags = flags;
	this->addr_space = addr_space;
	this->stack_ptr = STACK_BASE;
	this->filetable = Filetable(3);

	for (int i = 0; i < SIGNAL_TABLE_SIZE; i++)
		ProcessManager::SetSignalHandler(this, i, SIG_DFL);

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

	PROCESS *AddProcess(PROCESS *proc, PROCESS *parent)
	{
		if (!proc)
			return 0;

		if (proc->id <= 0)
			AssignPid(proc);

		proc->parent = parent;
		proc->next = first;
		first = proc;

		if (parent)
		{
			if (parent->first_child)
			{
				proc->next_sibling = proc->first_child;
				proc->first_child = proc;
			}
			else
			{
				parent->first_child = proc;
				proc->next_sibling = 0;
			}
		}

		//Start threads
		THREAD *thread = proc->main_thread;
		while (thread)
		{
			Scheduler::InsertThread(thread);
			thread = thread->proc_next;
		}

		return proc;
	}

	void RemoveProcess(PROCESS *proc)
	{
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

			pflags_t flags = PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
			int blocks = 1;

			void *virt = (void *)proc->stack_ptr;
			proc->stack_ptr = (size_t)virt + blocks * BLOCK_SIZE;

			if (proc->stack_ptr > STACK_END)
			{
				debug_print("Out of stack memory\n");
				sys_halt();
			}

			void *phys = PMem::AllocBlocks(blocks);
			VMem::MapPages(virt, phys, blocks, flags);

			thread = Task::CreateUserThread(entry, (void *)proc->stack_ptr);
			break;
		}

		if (AddThread(proc, thread))
			return thread;

		return 0;
	}

	bool AddThread(PROCESS *proc, THREAD *thread)
	{
		thread->process = proc;
		thread->addr_space = proc->addr_space;

		//Insert into thread list
		if (proc->main_thread == 0)
		{
			proc->main_thread = thread;
			thread->proc_next = 0;
		}
		else
		{
			thread->proc_next = proc->main_thread->proc_next;
			proc->main_thread->proc_next = thread;
		}

		return true;
	}

	bool RemoveThread(THREAD *thread)
	{
		if (!thread || !thread->process)
			return false;

		PROCESS *proc = thread->process;
		THREAD *prev = 0;
		THREAD *t = proc->main_thread;

		while (t)
		{
			if (t == thread)
			{
				if (prev)
					prev->proc_next = t->proc_next;
				else
					proc->main_thread = t->proc_next;

				break;
			}

			prev = t;
			t = t->proc_next;
		}

		return true;
	}

	bool StopThreads(PROCESS *proc)
	{
		THREAD *thread = proc->main_thread;
		while (thread)
		{
			Scheduler::ExitThread(0, thread);
			thread = thread->proc_next;
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
			FILE *file = proc->filetable.Get(fd);
			proc->filetable.Remove(fd);

			if (file)
			{
				if (VFS::Close(proc->filetable, fd) == -1)
					return false;
			}
		}

		return true;
	}

	void Terminate(PROCESS *proc)
	{
		ADDRESS_SPACE old_space = VMem::GetAddressSpace();
		VMem::SwapAddressSpace(proc->addr_space);
		FreeAllMemory(proc);
		CloseFiles(proc);
		StopThreads(proc);
		VMem::SwapAddressSpace(old_space);
	}

	void *AdjustHeap(PROCESS *proc, int increment)
	{
		size_t prev_end = proc->heap_end;
		size_t next_end = prev_end + increment;
		int cur_block = (prev_end - 1) / BLOCK_SIZE + 1;
		int next_block = (next_end - 1) / BLOCK_SIZE + 1;
		int blocks = next_block - cur_block;

		//debug_print("sbrk pid:%d %d=0x%x %i %i %p-%p\n", proc->id, increment, increment, BYTES_TO_BLOCKS(increment), blocks, prev_end, next_end);

		if (next_end > HEAP_END)
			return 0;

		if (blocks > 0)
		{
			pflags_t flags = PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
			size_t virt = cur_block * BLOCK_SIZE;
			void *phys = PMem::AllocBlocks(blocks);
			VMem::MapPages((void *)virt, phys, blocks, flags);

			//debug_print("%p-%p -> %p-%p  %p\n", virt, virt + blocks * BLOCK_SIZE, phys, (size_t)phys + blocks * BLOCK_SIZE, next_end);
		}
		else if (blocks < 0)
		{
			// TODO
			void *virt = (void *)((cur_block - blocks) * BLOCK_SIZE);
			VMem::FreePages(virt, blocks);
		}

		proc->heap_end = next_end;
		return (void *)prev_end;
	}

	void Exit(PROCESS *proc, int code)
	{
		Scheduler::Disable();
		debug_print("Exit code:%d pid:%d\n", code, proc->id);

		proc->siginfo.si_pid = proc->id;
		proc->siginfo.si_status = code;
		proc->siginfo.si_code = CLD_EXITED;
		proc->state = PROCESS_STATE_ZOMBIE;

		Scheduler::Enable();

		Dispatcher::HandleSignal(proc);
		Terminate(proc);
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