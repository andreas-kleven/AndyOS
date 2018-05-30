#include "syscalls.h"
#include "HAL/hal.h"
#include "debug.h"

int exit(int code)
{
	_asm cli
	_asm hlt
}

int print(char* text)
{
	Debug::Print(text);
	return 1;
}

int _cdecl gettime(int& hour, int& minute, int& second)
{
	hour = RTC::Hour();
	minute = RTC::Minute();
	second = RTC::Second();
	return 1;
}

void* syscalls[] =
{
	0,
	&exit,
	&print,
	&gettime
};

STATUS Syscalls::Init()
{
	return IDT::InstallIRQ(SYSCALL_IRQ, (IRQ_HANDLER)ISR);
}

void Syscalls::ISR(REGS* regs)
{
	if (regs->eax > sizeof(syscalls))
		return;

	void* location = syscalls[regs->eax];

	if (!location)
		return;

	int p1 = regs->edi;
	int p2 = regs->esi;
	int p3 = regs->edx;
	int p4 = regs->ecx;
	int p5 = regs->ebx;

	uint32 result;

	_asm
	{
		push p1
		push p2
		push p3
		push p4
		push p5
		call location
		mov[result], eax
		add esp, 40
	}

	regs->eax = result;
}
