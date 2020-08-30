#pragma once
#include <Process/process.h>
#include <types.h>

namespace ProcessManager::Arch {
void HandleSignal(THREAD *thread, int signo, sig_t handler);
void FinishSignal(THREAD *thread);
}; // namespace ProcessManager::Arch
