#pragma once
#include "definitions.h"
#include "HAL/idt.h"

typedef void(*SYSCALL_HANDLER)();

namespace Syscalls
{
	void InstallSyscall(int id, SYSCALL_HANDLER);
	STATUS Init();
};