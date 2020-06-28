#include "Arch/task.h"

namespace Task::Arch
{
    THREAD* CreateKernelThread(void(*entry)())
    {
        return 0;
    }

	THREAD* CreateUserThread(void(*entry)(), void* stack)
    {

    void Start(void(*entry)())
    {
    }
}