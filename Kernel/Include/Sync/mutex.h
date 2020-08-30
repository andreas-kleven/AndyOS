#pragma once
#include <Process/thread.h>
#include <queue.h>

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
