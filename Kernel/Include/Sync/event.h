#pragma once
#include <queue.h>

struct THREAD;

class Event
{
private:
    Queue<THREAD *> waiting;
    bool set;
    bool auto_reset;

public:
    Event(bool set = false, bool auto_reset = false);

    bool Wait(int timeout = 0, bool interruptible = false);
    bool WaitIntr();
    void Set();
    void Wake(THREAD *thread, bool timeout, bool interrupted);
    void Clear();
};
