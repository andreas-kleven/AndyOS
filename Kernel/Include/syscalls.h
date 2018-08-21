#pragma once
#include "types.h"

typedef void(*SYSCALL_HANDLER)();

namespace Syscalls
{
	void InstallSyscall(int id, SYSCALL_HANDLER);
	SYSCALL_HANDLER GetSyscall(int id);

	bool Init();
};