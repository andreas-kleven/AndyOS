#pragma once
#include "Process/thread.h"

namespace Task::Arch
{
    THREAD* CreateKernelThread(void(*entry)());
	THREAD* CreateUserThread(void(*entry)(), void* stack);
    THREAD* CopyThread(THREAD* thread, pid_t newpid);

    void Switch();
    void Start(void(*entry)());
}