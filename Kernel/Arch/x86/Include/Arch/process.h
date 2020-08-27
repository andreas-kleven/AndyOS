#pragma once
#include <types.h>
#include <Process/process.h>

namespace ProcessManager::Arch
{
    void HandleSignal(THREAD *thread, int signo, sig_t handler);
    void FinishSignal(THREAD *thread);
};
