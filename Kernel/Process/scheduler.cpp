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
	THREAD* first_thread;
	THREAD* last_thread;
	THREAD* current_thread;

	bool enabled = false;
	int disableCount = 0;

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
			Task::Switch();
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
				Task::Switch();
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
				Task::Switch();
		}
	}

	void AwakeThread(THREAD* thread)
	{
		if (thread->state == THREAD_STATE_TERMINATED)
			return;

		if (thread->state != THREAD_STATE_TERMINATED)
		{
			if (Timer::Ticks() >= thread->sleep_until)
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

	THREAD* CurrentThread()
	{
		return current_thread;
	}

	THREAD* Schedule()
	{
		if (!enabled)
			return current_thread;

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
			
			//Waiting
			if (current_thread->state == THREAD_STATE_SLEEPING)
			{
				if (Timer::Ticks() >= current_thread->sleep_until)
				{
					current_thread->sleep_until = 0;
					current_thread->state = THREAD_STATE_READY;
				}
			}

			if (current_thread->state == THREAD_STATE_READY || current_thread->state == THREAD_STATE_INITIALIZED)
				break;
		}

		VMem::SwapAddressSpace(current_thread->addr_space);
		current_thread->state = THREAD_STATE_RUNNING;

		return current_thread;
	}

	void Init()
	{
		first_thread = 0;
		last_thread = 0;
		current_thread = 0;
	}
}