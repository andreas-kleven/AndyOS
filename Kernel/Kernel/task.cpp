#include "task.h"
#include "Process/thread.h"
#include "Process/scheduler.h"
#include "HAL/hal.h"
#include "Memory/memory.h"
#include "string.h"

#include "Arch/idt.h"
#include "Arch/pit.h"
#include "Arch/tss.h"
#include "Arch/pic.h"

#define TASK_SCHEDULE_IRQ 32

namespace Task
{
	uint32 id_counter = 0;
	uint32 tmp_stack;

	uint8 __attribute__((aligned(16))) fpu_state[512];

	THREAD* CreateKernelThread(void(*entry)())
	{
        int stack_size = 0x1000;

		THREAD* thread = (THREAD*)((uint32)(new char[stack_size]) + stack_size - sizeof(THREAD));
		thread->regs = (REGS*)((uint32)thread - sizeof(REGS));
		thread->regs->ebp = 0;
		thread->regs->esp = 0;
		thread->regs->edi = 0;
		thread->regs->esi = 0;
		thread->regs->edx = 0;
		thread->regs->ecx = 0;
		thread->regs->ebx = 0;
		thread->regs->eax = 0;
		thread->regs->user_stack = 0;
		thread->regs->user_ss = 0;
		thread->kernel_esp = 0;
		thread->addr_space.ptr = 0;
		thread->next = 0;
		thread->procNext = 0;

		thread->id = ++id_counter;
		thread->state = THREAD_STATE_INITIALIZED;
		thread->stack = (uint32)(thread - 1) - sizeof(REGS);
		thread->regs = (REGS*)thread->stack;
		thread->regs->esp = (uint32)&thread->regs;
		thread->regs->eip = (uint32)entry;
		thread->regs->eflags = 0x200;
		thread->regs->cs = KERNEL_CS;
		thread->regs->ds = KERNEL_SS;
		thread->regs->es = KERNEL_SS;
		thread->regs->fs = KERNEL_SS;
		thread->regs->gs = KERNEL_SS;

		thread->addr_space = VMem::GetAddressSpace();
		thread->fpu_state = new uint8[512];

		return thread;
	}

	THREAD* CreateUserThread(void(*entry)(), void* stack)
	{
		THREAD* thread = CreateKernelThread(entry);
		thread->kernel_esp = thread->stack;

		thread->regs->cs = USER_CS;
		thread->regs->ds = USER_SS;
		thread->regs->es = USER_SS;
		thread->regs->fs = USER_SS;
		thread->regs->gs = USER_SS;
		thread->regs->user_stack = (uint32)stack;
		thread->regs->user_ss = USER_SS;

		return thread;
	}

    void Switch()
    {
        asm volatile("int %0" :: "N" (TASK_SCHEDULE_IRQ));
    }

    void Schedule()
    {
        THREAD* current_thread = Scheduler::CurrentThread();

		PIT::ticks++;

		//Save stack
		current_thread->stack = tmp_stack;
		current_thread->regs = (REGS*)tmp_stack;

		asm volatile("fxsave (%0)" :: "m" (fpu_state));
		memcpy(current_thread->fpu_state, fpu_state, 512);
        
        //
        current_thread = Scheduler::Schedule();

		//Restore stack
		tmp_stack = current_thread->stack;

		TSS::SetStack(KERNEL_SS, current_thread->kernel_esp);
		PIC::InterruptDone(TASK_SCHEDULE_IRQ);
    }

	void INTERRUPT Task_ISR()
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
			"call %P1\n"

			//Load registers
			"mov %2, %%esp\n"

			"pop %%gs\n"
			"pop %%fs\n"
			"pop %%es\n"
			"pop %%ds\n"
			"popa\n"

			"iret"
			: "=m" (tmp_stack) : "i" (&Schedule), "m" (tmp_stack));
	}

	void Start(void(*entry)())
	{
        THREAD* thread = CreateKernelThread(entry);
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
			"iret" :: "r" (thread->stack));
	}
}