#include "sync.h"
#include "scheduler.h"

Event::Event(bool set)
{
    this->set = set;
}

void Event::Wait()
{
    if (!this->set)
    {
        THREAD* thread = Scheduler::CurrentThread();
        waiting.Enqueue(thread);
        Scheduler::BlockThread(thread);
    }
}

void Event::Set()
{
    while (waiting.Count() > 0)
    {
        THREAD* thread = waiting.Dequeue();
        Scheduler::WakeThread(thread);
    }

    this->set = true;
}

void Event::Clear()
{
    this->set = false;
}