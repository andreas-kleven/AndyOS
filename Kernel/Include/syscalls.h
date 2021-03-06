#pragma once
#include <Process/dispatcher.h>
#include <types.h>

typedef void (*SYSCALL_HANDLER)();

namespace Syscalls {
void InstallSyscall(int id, SYSCALL_HANDLER);
SYSCALL_HANDLER GetSyscall(int id);
void DoSyscall(DISPATCHER_CONTEXT &context, bool noreturn);

bool Init();
}; // namespace Syscalls
