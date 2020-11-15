#pragma once
#include <Process/dispatcher.h>

namespace Syscalls::Arch {
void ReturnSyscall(DISPATCHER_CONTEXT &context, int value);
void Init();
} // namespace Syscalls::Arch
