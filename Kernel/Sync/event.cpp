#include "sync.h"
#include "Process/scheduler.h"

Event::Event(bool set, bool auto_reset)
{
    this->set = set;
    this->auto_reset = auto_reset;
}

void Event::Wait()
{
    Scheduler::Disable();

    if (!set)
    {
        THREAD *thread = Scheduler::CurrentThread();
        waiting.Enqueue(thread);
        Scheduler::BlockThread(thread, false);
        Scheduler::Enable();
        Scheduler::Switch();
    }
    else
    {
        if (auto_reset)
            set = false;

        Scheduler::Enable();
    }
}

void Event::Set()
{
    Scheduler::Disable();

    if (!auto_reset || waiting.Count() == 0)
        set = true;

    while (waiting.Count() > 0)
    {
        THREAD *thread = waiting.Dequeue();
        Scheduler::WakeThread(thread);
    }

    Scheduler::Enable();
}

void Event::Clear()
{
    set = false;
}
