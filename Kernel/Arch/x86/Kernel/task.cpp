#include "Arch/task.h"
#include "Arch/idt.h"
#include "Arch/pit.h"
#include "Arch/tss.h"
#include "Arch/pic.h"
#include "Process/thread.h"
#include "Process/scheduler.h"
#include "Memory/memory.h"
#include "syscalls.h"
#include "syscall_list.h"
#include "hal.h"
#include "string.h"

#define TASK_SCHEDULE_IRQ 32

namespace Task::Arch
{
	const int stack_size = 0x1000;

	size_t tmp_stack;
	uint8 __attribute__((aligned(16))) fpu_state[512];

	THREAD *CreateKernelThread(void (*entry)())
	{
		THREAD *thread = (THREAD *)((size_t)(new char[stack_size]) + stack_size - sizeof(THREAD));

		thread->stack = (size_t)(thread - 1) - sizeof(REGS);
		thread->state = THREAD_STATE_INITIALIZED;
		thread->fpu_state = new uint8[512];
		thread->kernel_esp = 0;
		thread->next = 0;
		thread->procNext = 0;
		thread->sleep_until = 0;

		REGS *regs = (REGS *)thread->stack;
		regs->ebp = 0;
		regs->edi = 0;
		regs->esi = 0;
		regs->edx = 0;
		regs->ecx = 0;
		regs->ebx = 0;
		regs->eax = 0;
		regs->user_stack = 0;
		regs->user_ss = 0;
		regs->esp = thread->stack;
		regs->eip = (uint32)entry;
		regs->eflags = 0x200;
		regs->cs = KERNEL_CS;
		regs->ds = KERNEL_SS;
		regs->es = KERNEL_SS;
		regs->fs = KERNEL_SS;
		regs->gs = KERNEL_SS;
		return thread;
	}

	THREAD *CreateUserThread(void (*entry)(), void *stack)
	{
		THREAD *thread = CreateKernelThread(entry);
		thread->kernel_esp = thread->stack;

		REGS *regs = (REGS *)thread->stack;
		regs->cs = USER_CS;
		regs->ds = USER_SS;
		regs->es = USER_SS;
		regs->fs = USER_SS;
		regs->gs = USER_SS;
		regs->user_stack = (uint32)stack;
		regs->user_ss = USER_SS;

		return thread;
	}

	THREAD *CopyThread(THREAD *old)
	{
		//Copy stack
		char *old_stack = (char *)old - stack_size + sizeof(THREAD);
		char *new_stack = new char[stack_size];
		memcpy(new_stack, old_stack, stack_size);

		ssize_t offset = (ssize_t)new_stack - (ssize_t)old_stack;
		THREAD *thread = (THREAD *)((size_t)old + offset);
		thread->stack = old->stack + offset;
		thread->kernel_esp = old->kernel_esp + offset;

		//Copy fpu state
		thread->fpu_state = new uint8[512];
		memcpy(thread->fpu_state, old->fpu_state, 512);

		return thread;
	}

	int SetThreadReturn(THREAD *thread, int ret)
	{
		REGS *regs = (REGS *)thread->stack;
		regs->eax = ret;
		return 1;
	}

	void ScheduleTask(bool syscall)
	{
		if (!syscall)
			PIT::ticks++;

		THREAD *current_thread = Scheduler::CurrentThread();

		//Save stack
		current_thread->stack = tmp_stack;

		//Save fpu state
		//asm volatile("fxsave (%0)" :: "m" (fpu_state));
		//memcpy(current_thread->fpu_state, fpu_state, 512);

		if (syscall)
		{
			REGS *regs = (REGS *)current_thread->stack;

			void *location = (void *)Syscalls::GetSyscall(regs->eax);

			if (!location)
				panic("Invalid syscall", "Id: %i", regs->eax);

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
				: "g"(regs->ebp), "r"(regs->edi), "r"(regs->esi), "r"(regs->edx), "r"(regs->ecx), "r"(regs->ebx), "r"(location));

			regs->eax = ret;
		}

		//Schedule
		current_thread = Scheduler::Schedule();

		//Restore fpu state
		/*if (current_thread->state != THREAD_STATE_INITIALIZED)
		{
			memcpy(fpu_state, current_thread->fpu_state, 512);
			asm volatile("fxrstor (%0)" : "=m" (fpu_state));
		}*/

		//Restore stack
		tmp_stack = current_thread->stack;

		TSS::SetStack(KERNEL_SS, current_thread->kernel_esp);

		if (syscall)
			PIC::InterruptDone(SYSCALL_IRQ);
		else
			PIC::InterruptDone(TASK_SCHEDULE_IRQ);
	}

	void INTERRUPT Task_ISR()
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
			"push $0x0\n"
			"call %P1\n"

			//Load registers
			"mov %2, %%esp\n"

			"pop %%gs\n"
			"pop %%fs\n"
			"pop %%es\n"
			"pop %%ds\n"
			"popa\n"

			"iret"
			: "=m"(tmp_stack)
			: "i"(&ScheduleTask), "m"(tmp_stack));
	}

	void Start(void (*entry)())
	{
		THREAD *thread = CreateKernelThread(entry);
		Scheduler::InsertThread(thread);

		disable();
		IDT::SetISR(TASK_SCHEDULE_IRQ, Task_ISR, 0);

		asm volatile(
			"mov %0, %%esp\n"
			"pop %%gs\n"
			"pop %%fs\n"
			"pop %%es\n"
			"pop %%ds\n"
			"popa\n"
			"iret" ::"r"(thread->stack));
	}
} // namespace Task::Arch
