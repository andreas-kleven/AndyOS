#pragma once
#include "thread.h"

#define DISPATCHER_THREADS 32

struct DISPATCHER_CONTEXT
{
    THREAD *thread;
    int syscall;
    int p1;
    int p2;
    int p3;
    int p4;
    int p5;
    int p6;
};

struct DISPATCHER_ENTRY
{
    THREAD *handler_thread;
    DISPATCHER_CONTEXT context;

    bool operator==(const DISPATCHER_ENTRY &other) const
    {
        return context.thread == other.context.thread;
    }
};

namespace Dispatcher
{
    void Start();
    void Dispatch(const DISPATCHER_CONTEXT &context);
    DISPATCHER_ENTRY *CurrentDispatcherThread();
    THREAD *CurrentThread();
    PROCESS *CurrentProcess();
} // namespace Dispatcher
