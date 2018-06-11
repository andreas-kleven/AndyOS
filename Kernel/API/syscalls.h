#pragma once
#include "definitions.h"
#include "HAL/idt.h"

static class Syscalls
{
public:
	static STATUS Init();
	static void InstallSyscall(int id, void* handler);

private:
	static void ISR(REGS* regs);
};