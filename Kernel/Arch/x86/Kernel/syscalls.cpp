#include <syscalls.h>
#include <Arch/scheduler.h>
#include <Arch/idt.h>
#include <Kernel/task.h>
#include <syscall_list.h>
#include <debug.h>

namespace Syscalls::Arch
{
	void DoSyscall(DISPATCHER_CONTEXT &context, void *location)
	{
		THREAD *thread = context.thread;

		uint32 ret;

		asm volatile(
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
			: "=a"(ret)
			: "g"(context.p6), "r"(context.p5), "r"(context.p4), "r"(context.p3), "r"(context.p2), "r"(context.p1), "r"(location));

		REGS *regs = (REGS *)thread->stack;
		regs->eax = ret;
	}

	void INTERRUPT Syscall_ISR()
	{
		asm volatile(
			"cli\n"
			
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
			: "=m"(Scheduler::Arch::tmp_stack)
			: "i"(&Scheduler::Arch::ScheduleTask), "m"(Scheduler::Arch::tmp_stack));
	}

	void Init()
	{
		IDT::SetISR(SYSCALL_IRQ, Syscall_ISR, IDT_DESC_RING3);
	}
} // namespace Syscalls::Arch
