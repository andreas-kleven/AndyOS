#include "thread.h"
#include "Memory/memory.h"

Thread::Thread()
{
	this->regs.ebp = 0;
	this->regs.esp = 0;
	this->regs.edi = 0;
	this->regs.esi = 0;
	this->regs.edx = 0;
	this->regs.ecx = 0;
	this->regs.ebx = 0;
	this->regs.eax = 0;
}
