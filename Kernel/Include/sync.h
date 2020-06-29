#pragma once
#include "queue.h"
#include "Process/thread.h"

class Event
{
private:
    Queue<THREAD *> waiting;
    bool set;

public:
    Event(bool set = false);

    void Wait();
    void Set();
    void Clear();
};
