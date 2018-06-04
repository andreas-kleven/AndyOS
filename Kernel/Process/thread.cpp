#include "thread.h"
#include "Memory/memory.h"
#include "string.h"

THREAD::THREAD()
{
	this->regs->ebp = 0;
	this->regs->esp = 0;
	this->regs->edi = 0;
	this->regs->esi = 0;
	this->regs->edx = 0;
	this->regs->ecx = 0;
	this->regs->ebx = 0;
	this->regs->eax = 0;
	this->regs->user_stack = 0;
	this->regs->user_ss = 0;

	this->kernel_esp = 0;
	this->page_dir = 0;
	this->next = 0;
	this->procNext = 0;

	memset(this->fpu_state, 0, sizeof(this->fpu_state));
}
