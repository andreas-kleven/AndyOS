#include "task.h"
#include "Arch/task.h"

namespace Task
{
	THREAD* CreateKernelThread(void(*entry)())
	{
		Arch::CreateKernelThread(entry);
	}

	THREAD* CreateUserThread(void(*entry)(), void* stack)
	{
		Arch::CreateUserThread(entry, stack);
	}

	THREAD* CopyThread(THREAD* thread, pid_t newpid)
	{
		if (!thread)
			return 0;

		THREAD* nt = Arch::CopyThread(thread, newpid);
		nt->next = 0;
		nt->procNext = 0;
		nt->id = 0;
		
		return nt;
	}

	void Switch()
	{
		Arch::Switch();
	}

	void Start(void(*entry)())
	{
		Arch::Start(entry);
	}
}