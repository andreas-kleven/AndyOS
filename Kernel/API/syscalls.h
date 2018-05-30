#pragma once
#include "definitions.h"
#include "HAL/idt.h"

#define SYSCALL_IRQ 0x80

static class Syscalls
{
public:
	static STATUS Init();

private:
	static void ISR(REGS* regs);
};