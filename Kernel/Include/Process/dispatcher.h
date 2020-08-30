#pragma once
#include <Process/thread.h>
#include <siginfo.h>
#include <sync.h>
#include <wait.h>

#define DISPATCHER_THREADS 256

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

struct DISPATCHER_WAIT_INFO
{
    bool waiting = false;
    pid_t pid;
    idtype_t type;
    siginfo_t siginfo;
    Event event = Event(false, true);
};

struct DISPATCHER_ENTRY
{
    bool used = false;
    THREAD *handler_thread = 0;
    DISPATCHER_CONTEXT context;
    DISPATCHER_WAIT_INFO wait_info;

    bool operator==(const DISPATCHER_ENTRY &other) const
    {
        return context.thread == other.context.thread;
    }
};

namespace Dispatcher {
void Start();
void Dispatch(const DISPATCHER_CONTEXT &context);
DISPATCHER_ENTRY *CurrentEntry();
THREAD *CurrentThread();
PROCESS *CurrentProcess();
void HandleSignal(PROCESS *process);
int Waitpid(pid_t pid, int *status, int options);
DISPATCHER_ENTRY *GetEntryFor(THREAD *thread);
} // namespace Dispatcher
