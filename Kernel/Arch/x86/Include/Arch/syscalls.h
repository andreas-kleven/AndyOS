#pragma once
#include <Process/dispatcher.h>

namespace Syscalls::Arch
{
    int DoSyscall(DISPATCHER_CONTEXT &context, void *location);
    void ReturnSyscall(DISPATCHER_CONTEXT &context, int value);
    void Init();
} // namespace Syscalls::Arch
