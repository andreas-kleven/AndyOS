#pragma once
#include <Process/thread.h>

namespace Task::Arch
{
    THREAD *ResetKernelThread(THREAD *thread, void (*entry)());
    THREAD *CreateKernelThread(void (*entry)());
    THREAD *CreateUserThread(void (*entry)(), void *stack);
    THREAD *CopyThread(THREAD *thread);
    int SetThreadReturn(THREAD *thread, int ret);
} // namespace Task::Arch