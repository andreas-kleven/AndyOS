#include "syscalls.h"
#include "HAL/hal.h"
#include "debug.h"

void* syscalls[MAX_SYSCALLS];

int halt(int code)
{
	_asm cli
	_asm hlt
}

int print(char* text)
{
	Debug::Print(text);
	return 1;
}

int color(uint32 color)
{
	Debug::color = color;
	return 1;
}

int gettime(int& hour, int& minute, int& second)
{
	hour = RTC::Hour();
	minute = RTC::Minute();
	second = RTC::Second();
	return 1;
}

STATUS Syscalls::Init()
{
	if (!IDT::InstallIRQ(SYSCALL_IRQ, (IRQ_HANDLER)ISR))
		return STATUS_FAILED;

	InstallSyscall(SYSCALL_HALT, halt);
	InstallSyscall(SYSCALL_PRINT, print);
	InstallSyscall(SYSCALL_COLOR, color);
	InstallSyscall(SYSCALL_GETTIME, gettime);
}

void Syscalls::InstallSyscall(int id, void* handler)
{
	if (id <= 0 || id >= MAX_SYSCALLS)
		return;

	syscalls[id] = handler;
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
