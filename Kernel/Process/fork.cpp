#include "process.h"
#include "dispatcher.h"
#include "scheduler.h"
#include "Kernel/task.h"
#include "string.h"

namespace ProcessManager
{
	bool CopyThreads(PROCESS *proc, PROCESS *newproc)
	{
		THREAD *current_thread = Dispatcher::CurrentThread();
		THREAD *thread = proc->main_thread;

		while (thread)
		{
			if (thread->state != THREAD_STATE_TERMINATED)
			{
				THREAD *copy = Task::CopyThread(thread);

				if (!copy)
					return false;

				if (!AddThread(newproc, copy))
					return false;

				if (thread == current_thread)
					Task::SetThreadReturn(copy, 0);
			}

			thread = thread->proc_next;
		}

		return true;
	}

	PROCESS *Fork(PROCESS *proc)
	{
		debug_print("Fork %d\n", proc->id);

		VMem::SwapAddressSpace(proc->addr_space);
		ADDRESS_SPACE space;

		if (!VMem::CopyAddressSpace(&space))
			return 0;

		VMem::SwapAddressSpace(space);

		PROCESS *newproc = new PROCESS(proc->flags, space);
		AssignPid(newproc);

		memcpy(newproc->signal_table, proc->signal_table, SIGNAL_TABLE_SIZE);
		newproc->message_handler = proc->message_handler;

		for (int i = 0; i < FILE_TABLE_SIZE; i++)
		{
			if (proc->file_table[i])
				newproc->file_table[i] = new FILE(*proc->file_table[i]);
		}

		THREAD *current_thread = Scheduler::CurrentThread();

		if (!CopyThreads(proc, newproc))
		{
			//Todo: cleanup
			return 0;
		}

		VMem::SwapAddressSpace(proc->addr_space);

		if (!AddProcess(newproc))
			return 0;

		debug_print("Fork complete\n");
		return newproc;
	}
} // namespace ProcessManager
