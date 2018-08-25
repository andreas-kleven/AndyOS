#include "Arch/syscalls.h"
#include "Arch/idt.h"
#include "Process/process.h"
#include "Process/scheduler.h"
#include "panic.h"
#include "syscalls.h"
#include "syscall_list.h"
#include "debug.h"

namespace Syscalls::Arch
{
	void UpdateStack(REGS* regs)
	{
		THREAD* t = Scheduler::CurrentThread();
		t->stack = regs->esp + 20 - sizeof(REGS);
	}

    void Handler(REGS* regs)
    {
		void* location = (void*)Syscalls::GetSyscall(regs->eax);

		if (!location)
			panic("Invalid syscall", "IRQ: %i", regs->eax);

		UpdateStack(regs);

		uint32 ret;

		asm volatile (
			"push %1\n"
			"push %2\n"
			"push %3\n"
			"push %4\n"
			"push %5\n"
			"push %6\n"
			"call *%7\n"
			"pop %%ebx\n"
			"pop %%ebx\n"
			"pop %%ebx\n"
			"pop %%ebx\n"
			"pop %%ebx\n"
			"pop %%ebx\n"
			: "=a" (ret) : "g" (regs->ebp), "r" (regs->edi), "r" (regs->esi), "r" (regs->edx), "r" (regs->ecx), "r" (regs->ebx), "r" (location));

		regs->eax = ret;
    }

    void Init()
    {
        IDT::InstallIRQ(SYSCALL_IRQ, Handler);
    }
}