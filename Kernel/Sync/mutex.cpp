#include <Process/scheduler.h>
#include <sync.h>

Mutex::Mutex()
{
    this->thread = 0;
    this->aquire_count = 0;
}

void Mutex::Aquire()
{
    Scheduler::Disable();

    THREAD *current_thread = Scheduler::CurrentThread();

    if (current_thread != this->thread && this->aquire_count > 0) {
        waiting.Enqueue(current_thread);
        Scheduler::BlockThread(current_thread, false);
        Scheduler::Enable();
        Scheduler::Switch();
    } else {
        this->thread = current_thread;
        this->aquire_count += 1;
        Scheduler::Enable();
    }
}

void Mutex::Release()
{
    Scheduler::Disable();

    this->aquire_count -= 1;

    if (this->aquire_count == 0) {
        if (waiting.Count() > 0) {
            THREAD *next = waiting.Dequeue();
            Scheduler::WakeThread(next);

            this->thread = next;
            this->aquire_count += 1;
        }
    }

    Scheduler::Enable();
}
