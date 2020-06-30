#include "dispatcher.h"
#include "Kernel/task.h"
#include "Process/scheduler.h"
#include "hal.h"
#include "syscalls.h"
#include "queue.h"
#include "list.h"
#include "sync.h"
#include "string.h"

namespace Dispatcher
{
    DISPATCHER_ENTRY entries[DISPATCHER_THREADS];
    Queue<DISPATCHER_CONTEXT> queue = Queue<DISPATCHER_CONTEXT>();
    Event queue_event;
    Event thread_event = Event(true);
    int active_count;

    DISPATCHER_ENTRY *GetFirstAvailable()
    {
        Scheduler::Disable();

        for (int i = 0; i < DISPATCHER_THREADS; i++)
        {
            DISPATCHER_ENTRY *entry = &entries[i];

            if (entry->handler_thread && !entry->context.thread)
            {
                Scheduler::Enable();
                return entry;
            }
        }

        Scheduler::Enable();
        return 0;
    }

    DISPATCHER_ENTRY *GetFirstFree()
    {
        Scheduler::Disable();

        for (int i = 0; i < DISPATCHER_THREADS; i++)
        {
            if (entries[i].handler_thread == 0)
            {
                Scheduler::Enable();
                return &entries[i];
            }
        }

        Scheduler::Enable();
        return 0;
    }

    void ResetThread(DISPATCHER_ENTRY *entry)
    {
        Scheduler::Disable();
        active_count -= 1;
        entry->context.thread = 0;
        queue_event.Clear();
        thread_event.Set();
        Scheduler::Enable();
    }

    void HandlerFunc()
    {
        DISPATCHER_ENTRY *entry = CurrentDispatcherThread();
        DISPATCHER_CONTEXT &context = entry->context;
        THREAD *handler_thread = entry->handler_thread;
        THREAD *thread = context.thread;

        VMem::SwapAddressSpace(thread->process->addr_space);
        Syscalls::DoSyscall(context);
        Scheduler::WakeThread(context.thread);
        ResetThread(entry);
        Scheduler::RemoveThread(handler_thread);
        Scheduler::Switch();

        debug_print("Dispatcher error\n");
        sys_halt();
    }

    DISPATCHER_ENTRY *CreateThread()
    {
        Scheduler::Disable();
        DISPATCHER_ENTRY *entry = GetFirstFree();

        if (!entry)
        {
            Scheduler::Enable();
            return 0;
        }

        THREAD *handler_thread = Task::CreateKernelThread(HandlerFunc);
        entry->handler_thread = handler_thread;

        Scheduler::Enable();
        return entry;
    }

    DISPATCHER_ENTRY *GetAvailableThread(const DISPATCHER_CONTEXT &context)
    {
        thread_event.Wait();
        Scheduler::Disable();

        DISPATCHER_ENTRY *entry = GetFirstAvailable();

        if (entry)
            Task::ResetKernelThread(entry->handler_thread, HandlerFunc);

        if (!entry)
        {
            entry = CreateThread();

            if (entry)
                debug_print("Created dispatcher thread %d\n", entry - &entries[0]);
        }

        if (entry)
        {
            entry->context = context;
            active_count += 1;

            if (active_count == DISPATCHER_THREADS)
                thread_event.Clear();

            Scheduler::Enable();
            return entry;
        }

        debug_print("No dispatcher threads available\n");
        sys_halt();
        return 0;
    }

    void Dispatch(const DISPATCHER_CONTEXT &context)
    {
        queue.Enqueue(context);
        queue_event.Set();
    }

    void Start()
    {
        active_count = 0;

        memset(entries, 0, sizeof(entries));

        debug_print("Started dispatcher\n");

        while (true)
        {
            queue_event.Wait();
            Scheduler::Disable();

            if (queue.Count() == 0)
            {
                queue_event.Clear();
                Scheduler::Enable();
            }
            else
            {
                DISPATCHER_CONTEXT context = queue.Dequeue();
                Scheduler::Enable();

                DISPATCHER_ENTRY *entry = GetAvailableThread(context);
                Scheduler::InsertThread(entry->handler_thread);
            }
        }
    }

    DISPATCHER_ENTRY *CurrentDispatcherThread()
    {
        for (int i = 0; i < DISPATCHER_THREADS; i++)
        {
            DISPATCHER_ENTRY *entry = &entries[i];

            if (entry->handler_thread == Scheduler::CurrentThread())
                return entry;
        }

        return 0;
    }

    THREAD *CurrentThread()
    {
        DISPATCHER_ENTRY *entry = CurrentDispatcherThread();

        if (entry)
            return entry->context.thread;

        return 0;
    }

    PROCESS *CurrentProcess()
    {
        THREAD *thread = CurrentThread();

        if (thread)
            return thread->process;

        return 0;
    }
} // namespace Dispatcher
