#include "thread.h"
#include "Memory/memory.h"
#include "string.h"

THREAD::THREAD()
{
	this->regs.ebp = 0;
	this->regs.esp = 0;
	this->regs.edi = 0;
	this->regs.esi = 0;
	this->regs.edx = 0;
	this->regs.ecx = 0;
	this->regs.ebx = 0;
	this->regs.eax = 0;
	this->regs.user_stack = 0;
	this->regs.user_ss = 0;

	memset(this->fpu_state, 0, sizeof(this->fpu_state));
}
