#include "process.h"
#include "scheduler.h"
#include "Kernel/task.h"
#include "string.h"

namespace ProcessManager
{
	bool CopyThreads(PROCESS* proc, PROCESS* newproc)
	{
		THREAD* current_thread = Scheduler::CurrentThread();
		THREAD* thread = proc->main_thread;
		
		while (thread)
		{
			pid_t newpid = thread == current_thread ? newproc->id : 0;
			THREAD* copy = Task::CopyThread(thread, newpid);
			
			if (!copy)
				return false;

			if (!AddThread(newproc, copy))
				return false;

			if (thread == proc->main_thread)
				newproc->main_thread = copy;

			if (copy->state == THREAD_STATE_RUNNING)
				copy->state = THREAD_STATE_READY;

			thread = thread->procNext;
		}

		return true;
	}

	PROCESS* Fork(PROCESS* proc)
	{
		VMem::SwapAddressSpace(proc->addr_space);
		ADDRESS_SPACE space;
		
		if (!VMem::CopyAddressSpace(&space))
			return 0;

		VMem::SwapAddressSpace(space);

		PROCESS* newproc = new PROCESS(proc->flags, space);
		AssignPid(newproc);

		memcpy(newproc->file_table, proc->file_table, sizeof(proc->file_table));

		newproc->signal_handler = proc->signal_handler;
		newproc->message_handler = proc->message_handler;

		if (!CopyThreads(proc, newproc))
		{
			//Todo: cleanup
			return 0;
		}

		VMem::SwapAddressSpace(proc->addr_space);

		if (!AddProcess(newproc))
			return 0;

		return newproc;
	}
}