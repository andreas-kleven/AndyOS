#include "Arch/task.h"

namespace Task::Arch
{
    THREAD* CreateKernelThread(void(*entry)())
    {
        return 0;
    }

	THREAD* CreateUserThread(void(*entry)(), void* stack)
    {
        return 0;
    }

    void Switch()
    {
    }

    void Start(void(*entry)())
    {
    }
}