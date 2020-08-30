#include <Kernel/timer.h>
#include <Process/scheduler.h>
#include <sync.h>

Event::Event(bool set, bool auto_reset)
{
    this->set = set;
    this->auto_reset = auto_reset;
}

bool Event::Wait(int timeout, bool interruptible)
{
    Scheduler::Disable();

    THREAD *thread = Scheduler::CurrentThread();

    if (interruptible && thread->interrupted) {
        Scheduler::Enable();
        return false;
    }

    if (!set) {
        waiting.Enqueue(thread);

        thread->event = this;
        thread->event_interruptible = interruptible;

        if (timeout > 0)
            thread->event_until = Timer::Ticks() + timeout * 1000;
        else
            thread->event_until = 0;

        Scheduler::BlockThread(thread, false);
        Scheduler::Enable();
        Scheduler::Switch();

        if (thread->event_until) {
            thread->event_until = 0;
            return false; // timed out
        }

        if (thread->event_interruptible) {
            thread->event_interruptible = false;
            return false;
        }
    } else {
        if (auto_reset)
            set = false;

        Scheduler::Enable();
    }

    return true;
}

bool Event::WaitIntr()
{
    return Wait(0, true);
}

void Event::Set()
{
    Scheduler::Disable();

    if (!auto_reset || waiting.Count() == 0)
        set = true;

    while (waiting.Count() > 0) {
        THREAD *thread = waiting.Dequeue();

        if (thread->event) {
            thread->event = 0;
            thread->event_until = 0;
            thread->event_interruptible = false;
            Scheduler::WakeThread(thread);
        }
    }

    Scheduler::Enable();
}

void Event::Wake(THREAD *thread, bool timeout, bool interrupted)
{
    Scheduler::Disable();

    if (thread->event != this)
        panic("Event::Wake", "Invalid thread %d", thread ? thread->id : -1);

    thread->event = 0;

    if (!timeout)
        thread->event_until = 0;

    if (!interrupted)
        thread->event_interruptible = false;

    Scheduler::WakeThread(thread);
    Scheduler::Enable();
}

void Event::Clear()
{
    set = false;
}
