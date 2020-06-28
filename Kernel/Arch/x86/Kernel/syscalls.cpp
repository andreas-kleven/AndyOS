#include "Arch/syscalls.h"
#include "Arch/idt.h"
#include "Arch/pic.h"
#include "Arch/tss.h"
#include "Arch/task.h"
#include "Process/process.h"
#include "Process/scheduler.h"
#include "Kernel/task.h"
#include "panic.h"
#include "syscalls.h"
#include "syscall_list.h"
#include "debug.h"

namespace Syscalls::Arch
{
	void INTERRUPT Syscall_ISR()
	{
		asm volatile(
			//Save registers
			"pusha\n"
			"push %%ds\n"
			"push %%es\n"
			"push %%fs\n"
			"push %%gs\n"

			"mov %%esp, %0\n"

			//Schedule
			"push $0x1\n"
			"call %P1\n"

			//Load registers
			"mov %2, %%esp\n"

			"pop %%gs\n"
			"pop %%fs\n"
			"pop %%es\n"
			"pop %%ds\n"
			"popa\n"

			"iret"
			: "=m"(Task::Arch::tmp_stack)
			: "i"(&Task::Arch::ScheduleTask), "m"(Task::Arch::tmp_stack));
	}

	void Init()
	{
		IDT::SetISR(SYSCALL_IRQ, Syscall_ISR, IDT_DESC_RING3);
	}
} // namespace Syscalls::Arch
