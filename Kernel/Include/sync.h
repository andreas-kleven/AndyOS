#pragma once
#include "queue.h"
#include "Process/thread.h"

class Mutex
{
private:
    Queue<THREAD *> waiting;
    THREAD *thread;
    int aquire_count;

public:
    Mutex();

    void Aquire();
    void Release();
};

class Event
{
private:
    Queue<THREAD *> waiting;
    bool set;
    bool auto_reset;

public:
    Event(bool set = false, bool auto_reset = false);

    void Wait();
    void Set();
    void Clear();
};
