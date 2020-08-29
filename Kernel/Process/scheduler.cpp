#include <Process/scheduler.h>
#include <Arch/scheduler.h>
#include <hal.h>
#include <memory.h>
#include <Kernel/task.h>
#include <Kernel/timer.h>
#include <string.h>
#include <debug.h>

namespace Scheduler
{
	THREAD *first_thread;
	THREAD *last_thread;
	THREAD *current_thread;
	THREAD *idle_thread;

	bool is_interrupt = false;
	bool enabled = false;
	int disable_count = 0;

	void Enable()
	{
		if (disable_count == 1)
		{
			enabled = true;
			disable_count = 0;
		}
		else if (disable_count > 1)
		{
			disable_count--;
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
		disable_count++;
	}

	void InterruptEnter()
	{
		is_interrupt = true;
	}

	void InterruptExit()
	{
		is_interrupt = false;
	}

	int ReadyCount()
	{
		Disable();
		int count = 0;
		THREAD *thread = first_thread;

		while (thread)
		{
			if ((thread->state == THREAD_STATE_READY || thread->state == THREAD_STATE_RUNNING) && !thread->sleep_until)
				count++;

			thread = thread->next;

			if (thread == first_thread)
				break;
		}

		Enable();
		return count;
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

	void SleepThread(uint64 until, THREAD *thread)
	{
		Disable();

		if (thread->interrupted)
		{
			Enable();
			return;
		}

		if (thread->state != THREAD_STATE_TERMINATED)
		{
			thread->sleep_until = until;

			if (!is_interrupt && thread == current_thread)
			{
				Enable();
				Switch();
				return;
			}
		}

		Enable();
	}

	void BlockThread(THREAD *thread, bool auto_switch)
	{
		Disable();

		if (thread->state != THREAD_STATE_TERMINATED)
		{
			thread->state = THREAD_STATE_BLOCKING;

			if (!is_interrupt && auto_switch && thread == current_thread)
			{
				Enable();
				Switch();
				return;
			}
		}

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
			thread->prev = last_thread;
			thread->next = first_thread;
			thread->prev->next = thread;
			thread->next->prev = thread;
			last_thread = thread;
		}
		else
		{
			thread->next = thread;
			thread->prev = thread;
			first_thread = thread;
			last_thread = thread;
			current_thread = thread;
		}

		thread->inserted = true;

		Enable();
	}

	void RemoveThread(THREAD *thread)
	{
		if (!thread)
			return;

		Disable();

		THREAD *prev = thread->prev;
		THREAD *next = thread->next;
		thread->next->prev = prev;
		thread->prev->next = next;
		thread->prev = 0;
		thread->next = 0;

		if (next == thread)
			next = 0;
		if (prev == thread)
			prev = 0;

		if (thread == first_thread)
			first_thread = next;
		if (thread == last_thread)
			last_thread = prev;

		thread->inserted = false;

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

		current_thread->addr_space = VMem::GetAddressSpace();

		if (current_thread->state == THREAD_STATE_RUNNING)
			current_thread->state = THREAD_STATE_READY;

		//Schedule
		if (current_thread == idle_thread && first_thread)
			current_thread = first_thread;

		current_thread = current_thread->next;

		if (!current_thread)
			current_thread = first_thread;

		THREAD *first = current_thread;

		while (true)
		{
			while (current_thread && current_thread->state == THREAD_STATE_TERMINATED)
			{
				RemoveThread(current_thread);
				ProcessManager::RemoveThread(current_thread);
				THREAD *next = current_thread->next;
				//delete current_thread;
				current_thread = next;
			}

			if (!current_thread)
			{
				current_thread = idle_thread;
				break;
			}

			if (!current_thread->inserted)
			{
				debug_print("Not inserted %d %p %p\n", current_thread->id, current_thread);
				sys_halt();
			}

			// sleeping threads
			if (current_thread->sleep_until)
			{
				if (Timer::Ticks() >= current_thread->sleep_until)
					current_thread->sleep_until = 0;
			}

			// events
			if (current_thread->event && current_thread->event_until)
			{
				if (Timer::Ticks() >= current_thread->event_until)
					current_thread->event->Wake(current_thread, true, false);
			}

			if (current_thread->sleep_until == 0 && (current_thread->state == THREAD_STATE_READY || current_thread->state == THREAD_STATE_INITIALIZED) && (!current_thread->process || current_thread->process->state == PROCESS_STATE_RUNABLE))
				break;

			current_thread = current_thread->next;

			if (!current_thread || current_thread == first)
			{
				current_thread = idle_thread;
				break;
			}
		}

		if (current_thread->addr_space.ptr)
			VMem::SwapAddressSpace(current_thread->addr_space);

		current_thread->state = THREAD_STATE_RUNNING;
		return current_thread;
	}

	void Init()
	{
		first_thread = 0;
		last_thread = 0;
		current_thread = 0;

		idle_thread = Task::CreateKernelThread(Arch::Idle);
	}

	void Start(void (*entry)())
	{
		THREAD *thread = Task::CreateKernelThread(entry);
		Scheduler::InsertThread(thread);
		Arch::Start(thread);
	}

	void Switch()
	{
		if (!enabled)
			panic("Task switch", "Scheduler disabled");

		Arch::Switch();
	}
} // namespace Scheduler
