#pragma once
#include "types.h"
#include "Process/dispatcher.h"

typedef void (*SYSCALL_HANDLER)();

namespace Syscalls
{
	void InstallSyscall(int id, SYSCALL_HANDLER);
	SYSCALL_HANDLER GetSyscall(int id);
	void DoSyscall(DISPATCHER_CONTEXT &context);

	bool Init();
}; // namespace Syscalls
