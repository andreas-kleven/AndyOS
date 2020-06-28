#include "task.h"
#include "Arch/task.h"
#include "Lib/debug.h"

namespace Task
{
	THREAD *CreateKernelThread(void (*entry)())
	{
		return Arch::CreateKernelThread(entry);
	}

	THREAD *CreateUserThread(void (*entry)(), void *stack)
	{
		return Arch::CreateUserThread(entry, stack);
	}

	THREAD *CopyThread(THREAD *thread)
	{
		if (!thread)
			return 0;

		THREAD *nt = Arch::CopyThread(thread);
		nt->id = 0;
		nt->next = 0;
		nt->procNext = 0;
		nt->process = 0;

		if (thread->state == THREAD_STATE_RUNNING)
			nt->state = THREAD_STATE_READY;
		else
			nt->state = thread->state;

		return nt;
	}

	int SetThreadReturn(THREAD *thread, int ret)
	{
		return Arch::SetThreadReturn(thread, ret);
	}

	void Start(void (*entry)())
	{
		Arch::Start(entry);
	}
} // namespace Task
