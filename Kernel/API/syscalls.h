#pragma once
#include "definitions.h"
#include "HAL/idt.h"

#define SYSCALL_IRQ		0x80
#define MAX_SYSCALLS	1024

#define SYSCALL_HALT	1
#define SYSCALL_PRINT	2
#define SYSCALL_COLOR	3
#define SYSCALL_GETTIME	4

static class Syscalls
{
public:
	static STATUS Init();
	static void InstallSyscall(int id, void* handler);

private:
	static void ISR(REGS* regs);
};