#pragma once
#include "queue.h"
#include "Process/thread.h"

class Event
{
private:
    Queue<THREAD *> waiting;
    bool set;
    bool auto_reset;

public:
    Event(bool set = false, bool auto_reset = false);

    bool Wait(int timeout = 0);
    void Set();
    void Clear();
};
