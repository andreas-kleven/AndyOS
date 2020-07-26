#pragma once
#include <types.h>
#include <Process/process.h>

namespace ProcessManager::Arch
{
    void HandleSignal(PROCESS *proc, int signo, sig_t handler);
};
