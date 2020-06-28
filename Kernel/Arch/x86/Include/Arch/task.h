#pragma once
#include "Process/thread.h"

namespace Task::Arch
{
	extern size_t tmp_stack;

    THREAD* CreateKernelThread(void(*entry)());
	THREAD* CreateUserThread(void(*entry)(), void* stack);
    THREAD* CopyThread(THREAD* thread);
    int SetThreadReturn(THREAD* thread, int ret);
    void ScheduleTask(bool syscall);

    void Start(void(*entry)());
}