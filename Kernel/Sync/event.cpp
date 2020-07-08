#include "sync.h"
#include "Kernel/timer.h"
#include "Process/scheduler.h"

Event::Event(bool set, bool auto_reset)
{
    this->set = set;
    this->auto_reset = auto_reset;
}

bool Event::Wait(int timeout)
{
    Scheduler::Disable();

    if (!set)
    {
        THREAD *thread = Scheduler::CurrentThread();
        waiting.Enqueue(thread);

        thread->event = this;

        if (timeout > 0)
            thread->event_until = Timer::Ticks() + timeout * 1000;
        else
            thread->event_until = 0;

        Scheduler::BlockThread(thread, false);
        Scheduler::Enable();
        Scheduler::Switch();

        if (thread->event)
        {
            thread->event = 0;
            return false; // timed out
        }
    }
    else
    {
        if (auto_reset)
            set = false;

        Scheduler::Enable();
    }

    return true;
}

void Event::Set()
{
    Scheduler::Disable();

    if (!auto_reset || waiting.Count() == 0)
        set = true;

    while (waiting.Count() > 0)
    {
        THREAD *thread = waiting.Dequeue();
        thread->event = 0;
        thread->event_until = 0;
        Scheduler::WakeThread(thread);
    }

    Scheduler::Enable();
}

void Event::Clear()
{
    set = false;
}
