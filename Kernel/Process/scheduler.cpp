#include "scheduler.h"
#include "HAL/hal.h"
#include "Memory/memory.h"
#include "string.h"
#include "Lib/debug.h"

namespace Scheduler
{
	THREAD* first_thread;
	THREAD* last_thread;
	THREAD* idle_thread;
	THREAD* current_thread;

	uint32 id_counter = 0;
	uint32 tmp_stack;
	IRQ_HANDLER pit_isr;

	bool enabled = false;
	int disableCount = 0;

	uint8 __attribute__((aligned(16))) fpu_state[512];

	void Schedule()
	{
		//Call original isr
		pit_isr(0);

		if (!enabled)
		{
			PIC::InterruptDone(TASK_SCHEDULE_IRQ);
			return;
		}

		asm volatile("fxsave (%0)" :: "m" (fpu_state));
		memcpy(current_thread->fpu_state, fpu_state, 512);

		//Save stack
		current_thread->stack = tmp_stack;
		current_thread->regs = (REGS*)tmp_stack;

		if (current_thread->state == THREAD_STATE_RUNNING)
			current_thread->state = THREAD_STATE_READY;

		//Schedule
		THREAD* first = current_thread;

		while (current_thread->next != first)
		{
			current_thread = current_thread->next;

			while (current_thread->state == THREAD_STATE_TERMINATED)
			{
				THREAD* next = current_thread->next;
				RemoveThread(current_thread);	
				current_thread = next;
			}

			if (current_thread == idle_thread)
				current_thread = current_thread->next;

			//Waiting
			if (current_thread->state == THREAD_STATE_SLEEPING)
			{
				if (PIT::Ticks() >= current_thread->sleep_until)
				{
					current_thread->sleep_until = 0;
					current_thread->state = THREAD_STATE_READY;
				}
			}

			if (current_thread->state == THREAD_STATE_READY || current_thread->state == THREAD_STATE_INITIALIZED)
				break;
		}

		//Restore stack
		tmp_stack = current_thread->stack;

		VMem::SwapAddressSpace(current_thread->addr_space);
		TSS::SetStack(KERNEL_SS, current_thread->kernel_esp);
		PIC::InterruptDone(TASK_SCHEDULE_IRQ);

		//Restore fpu state
		if (current_thread->state != THREAD_STATE_INITIALIZED)
		{
			memcpy(fpu_state, current_thread->fpu_state, 512);
			asm volatile("fxrstor (%0)" : "=m" (fpu_state));
		}

		current_thread->state = THREAD_STATE_RUNNING;
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

	void Idle()
	{
		while (1)
		{
			//debug_print("Idle");
			pause();
		}
	}

	void Enable()
	{
		if (disableCount <= 1)
		{
			enabled = true;
			disableCount = 0;
		}
		else
		{
			disableCount--;
		}
	}

	void Disable()
	{
		enabled = false;
		disableCount++;
	}

	void ExitThread(int code, THREAD* thread)
	{
		thread->state = THREAD_STATE_TERMINATED;

		//Switch thread
		if (thread == current_thread)
			asm volatile("int %0" :: "N" (TASK_SCHEDULE_IRQ));
	}

	void SleepThread(uint32 until, THREAD* thread)
	{
		if (thread->state == THREAD_STATE_TERMINATED)
			return;

		if (thread->state != THREAD_STATE_TERMINATED)
		{
			thread->state = THREAD_STATE_SLEEPING;
			thread->sleep_until = until;

			//Switch thread
			if (thread == current_thread)
				asm volatile("int %0" :: "N" (TASK_SCHEDULE_IRQ));
		}
	}

	void BlockThread(THREAD* thread)
	{
		if (thread->state == THREAD_STATE_TERMINATED)
			return;

		if (thread->state != THREAD_STATE_TERMINATED)
		{
			thread->state = THREAD_STATE_BLOCKING;

			//Switch thread
			if (thread == current_thread)
				asm volatile("int %0" :: "N" (TASK_SCHEDULE_IRQ));
		}
	}

	void AwakeThread(THREAD* thread)
	{
		if (thread->state == THREAD_STATE_TERMINATED)
			return;

		if (thread->state != THREAD_STATE_TERMINATED)
		{
			if (PIT::Ticks() >= thread->sleep_until)
			{
				thread->state = THREAD_STATE_READY;
			}
			else
			{
				thread->state = THREAD_STATE_SLEEPING;
			}
		}
	}

	void InsertThread(THREAD* thread)
	{
		Disable();

		if (first_thread)
		{
			thread->next = first_thread;
			last_thread->next = thread;
			last_thread = thread;
		}
		else
		{
			thread->next = thread;
			first_thread = thread;
			last_thread = thread;
			current_thread = thread;
		}

		Enable();
	}

	void RemoveThread(THREAD* thread)
	{
		if (!thread)
			return;

		Disable();

		THREAD* t = first_thread;
		while (t)
		{
			if (t->next == thread)
			{
				if (thread == last_thread)
					last_thread = t;

				t->next = thread->next;
				delete thread;

				Enable();
				return;
			}

			t = t->next;
		}

		Enable();
	}

	THREAD* CreateKernelThread(void(*main)())
	{
		THREAD* thread = (THREAD*)((uint32)(new char[BLOCK_SIZE]) + BLOCK_SIZE - sizeof(THREAD));
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
		thread->regs->eip = (uint32)main;
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

	THREAD* CreateUserThread(void(*main)(), void* stack)
	{
		THREAD* thread = CreateKernelThread(main);
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

	THREAD* CurrentThread()
	{
		return current_thread;
	}

	void StartThreading()
	{
		disable();
		IDT::SetISR(TASK_SCHEDULE_IRQ, Task_ISR, 0);

		asm volatile(
			"mov %0, %%esp\n"
			"pop %%gs\n"
			"pop %%fs\n"
			"pop %%es\n"
			"pop %%ds\n"
			"popa\n"
			"iret" :: "r" (idle_thread->stack));
	}

	STATUS Init()
	{
		first_thread = 0;
		last_thread = 0;
		current_thread = 0;

		idle_thread = CreateKernelThread(Idle);
		InsertThread(idle_thread);

		pit_isr = IDT::GetHandler(TASK_SCHEDULE_IRQ);
		return STATUS_SUCCESS;
	}
}