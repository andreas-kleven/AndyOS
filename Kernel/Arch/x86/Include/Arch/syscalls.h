#pragma once
#include "Process/dispatcher.h"

namespace Syscalls::Arch
{
    void DoSyscall(DISPATCHER_CONTEXT &context, void *location);
    void Init();
} // namespace Syscalls::Arch
