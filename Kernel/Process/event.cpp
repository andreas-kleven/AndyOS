#include "sync.h"
#include "scheduler.h"

Event::Event(bool set)
{
    this->set = set;
}

void Event::Wait()
{
    Scheduler::Disable();

    if (!this->set)
    {
        THREAD *thread = Scheduler::CurrentThread();
        waiting.Enqueue(thread);
        Scheduler::BlockThread(thread, false);
        Scheduler::Enable();
        Scheduler::Switch();
    }
    else
    {
        Scheduler::Enable();
    }
}

void Event::Set()
{
    Scheduler::Disable();

    while (waiting.Count() > 0)
    {
        THREAD *thread = waiting.Dequeue();
        Scheduler::WakeThread(thread);
    }

    this->set = true;
    Scheduler::Enable();
}

void Event::Clear()
{
    this->set = false;
}
