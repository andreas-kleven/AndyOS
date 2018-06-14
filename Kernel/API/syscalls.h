#pragma once
#include "definitions.h"
#include "HAL/idt.h"

typedef void(*SYSCALL_HANDLER)();

class Syscalls
{
public:
	static STATUS Init();
	static void InstallSyscall(int id, SYSCALL_HANDLER);

private:
	static void ISR(REGS* regs);
};