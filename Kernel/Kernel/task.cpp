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

    void Switch()
    {
		Arch::Switch();
    }

    void Start(void(*entry)())
    {
		Arch::Start(entry);
    }
}