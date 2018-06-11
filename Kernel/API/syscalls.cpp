#include "syscalls.h"
#include "HAL/hal.h"
#include "string.h"
#include "debug.h"
#include "Drawing/drawing.h"
#include "Memory/memory.h"
#include "Process/scheduler.h"

void* syscalls[MAX_SYSCALLS];

void halt(int code)
{
	_asm cli
	_asm hlt
}

void print(char* text)
{
	Debug::Print(text);
}

void color(uint32 color)
{
	Debug::color = color;
}

void gettime(int& hour, int& minute, int& second)
{
	hour = RTC::Hour();
	minute = RTC::Minute();
	second = RTC::Second();
}

void draw(uint32* framebuffer)
{
	memcpy(Drawing::gc.framebuffer, framebuffer, Drawing::gc.memsize());
}

void exit(int code)
{
	Scheduler::ExitThread(code);
}

void sleep(uint32 ticks)
{
	Scheduler::SleepThread(PIT::ticks + ticks);

}

void* alloc(uint32 blocks)
{
	return VMem::UserAlloc(Scheduler::current_thread->page_dir, blocks);
}

void free(void* ptr, uint32 blocks)
{

}

STATUS Syscalls::Init()
{
	if (!IDT::InstallIRQ(SYSCALL_IRQ, (IRQ_HANDLER)ISR))
		return STATUS_FAILED;

	InstallSyscall(SYSCALL_HALT, halt);
	InstallSyscall(SYSCALL_PRINT, print);
	InstallSyscall(SYSCALL_COLOR, color);
	InstallSyscall(SYSCALL_GETTIME, gettime);
	InstallSyscall(SYSCALL_DRAW, draw);
	InstallSyscall(SYSCALL_EXIT, exit);
	InstallSyscall(SYSCALL_SLEEP, sleep);
	InstallSyscall(SYSCALL_ALLOC, alloc);
	InstallSyscall(SYSCALL_FREE, free);
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
