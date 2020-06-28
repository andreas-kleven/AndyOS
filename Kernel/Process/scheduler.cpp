#include "scheduler.h"
#include "hal.h"
#include "Memory/memory.h"
#include "Kernel/task.h"
#include "Kernel/timer.h"
#include "string.h"
#include "Lib/debug.h"

#include "Arch/regs.h"

namespace Scheduler
{
	THREAD *first_thread;
	THREAD *last_thread;
	THREAD *current_thread;
	THREAD *idle_thread;

	bool enabled = false;
	int disableCount = 0;

	void idle()
	{
		while (1)
			pause();
	}

	void Enable()
	{
		if (disableCount == 1)
		{
			enabled = true;
			disableCount = 0;
		}
		else if (disableCount > 1)
		{
			disableCount--;
		}
		else
		{
			debug_print("Scheduling error\n");
			sys_halt();
		}
	}

	void Disable()
	{
		enabled = false;
		disableCount++;
	}

	int id_counter = 1;
	int AssignId(THREAD *thread)
	{
		if (!thread)
			return 0;

		thread->id = id_counter++;
		return thread->id;
	}

	void ExitThread(int code, THREAD *thread)
	{
		thread->state = THREAD_STATE_TERMINATED;
	}

	void SleepThread(size_t until, THREAD *thread)
	{
		Disable();

		if (thread->state != THREAD_STATE_TERMINATED)
			thread->sleep_until = until;

		Enable();
	}

	void BlockThread(THREAD *thread)
	{
		Disable();

		if (thread->state != THREAD_STATE_TERMINATED)
			thread->state = THREAD_STATE_BLOCKING;

		Enable();
	}

	void WakeThread(THREAD *thread)
	{
		Disable();

		if (thread->state != THREAD_STATE_TERMINATED)
			thread->state = THREAD_STATE_READY;

		Enable();
	}

	void InsertThread(THREAD *thread)
	{
		Disable();
		AssignId(thread);

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

	void RemoveThread(THREAD *thread)
	{
		if (!thread)
			return;

		Disable();

		ProcessManager::RemoveThread(thread);

		THREAD *t = first_thread;
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

	THREAD *CurrentThread()
	{
		return current_thread;
	}

	THREAD *Schedule()
	{
		if (!enabled)
			return current_thread;

		if (current_thread->state == THREAD_STATE_RUNNING)
			current_thread->state = THREAD_STATE_READY;

		//Schedule
		if (current_thread == idle_thread && first_thread != 0)
			current_thread = first_thread;

		THREAD *first = current_thread;

		while (1)
		{
			current_thread = current_thread->next;

			if (current_thread == first)
			{
				if (current_thread->sleep_until != 0 || (current_thread->state != THREAD_STATE_READY && current_thread->state != THREAD_STATE_INITIALIZED && current_thread->state != THREAD_STATE_RUNNING))
				{
					current_thread = idle_thread;
				}
				break;
			}

			while (current_thread->state == THREAD_STATE_TERMINATED)
			{
				THREAD *next = current_thread->next;
				RemoveThread(current_thread);
				current_thread = next;
			}

			//Waiting
			if (current_thread->sleep_until)
			{
				if (Timer::Ticks() >= current_thread->sleep_until)
					current_thread->sleep_until = 0;
			}

			if (current_thread->sleep_until == 0 && (current_thread->state == THREAD_STATE_READY || current_thread->state == THREAD_STATE_INITIALIZED))
				break;
		}

		if (current_thread->process)
			VMem::SwapAddressSpace(current_thread->process->addr_space);

		current_thread->state = THREAD_STATE_RUNNING;

		return current_thread;
	}

	void Init()
	{
		first_thread = 0;
		last_thread = 0;
		current_thread = 0;

		idle_thread = Task::CreateKernelThread(idle);
	}
} // namespace Scheduler
