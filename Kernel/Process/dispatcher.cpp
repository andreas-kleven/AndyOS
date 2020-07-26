#include "dispatcher.h"
#include "Kernel/task.h"
#include "Process/scheduler.h"
#include "hal.h"
#include "syscalls.h"
#include "queue.h"
#include "list.h"
#include "sync.h"
#include "string.h"
#include "errno.h"

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

            if (entry->handler_thread && !entry->used)
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
        entry->used = false;
        queue_event.Clear();
        thread_event.Set();
        Scheduler::Enable();
    }

    void HandlerFunc()
    {
        DISPATCHER_ENTRY *entry = CurrentEntry();
        DISPATCHER_CONTEXT &context = entry->context;
        THREAD *handler_thread = entry->handler_thread;
        THREAD *thread = context.thread;

        VMem::SwapAddressSpace(thread->process->addr_space);
        Syscalls::DoSyscall(context);

        Scheduler::Disable();
        Scheduler::WakeThread(thread);
        ResetThread(entry);
        Scheduler::RemoveThread(handler_thread);
        Scheduler::Enable();
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
            entry->used = true;
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

    DISPATCHER_ENTRY *CurrentEntry()
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
        DISPATCHER_ENTRY *entry = CurrentEntry();

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

    siginfo_t GetSiginfo(DISPATCHER_ENTRY *entry, PROCESS *child, bool consume)
    {
        siginfo_t info;
        info.si_code = 0;

        if (!entry)
            return info;

        DISPATCHER_WAIT_INFO *wait = &entry->wait_info;
        PROCESS *parent = entry->context.thread->process;

        if (child->parent != parent || !SIGINFO_ANY(child->siginfo))
            return info;

        if (wait->type == P_ALL)
        {
            info = child->siginfo;

            if (consume)
                child->siginfo.si_code = 0;
        }
        else if (wait->type == P_PID)
        {
            if (wait->pid == child->id)
            {
                info = child->siginfo;

                if (consume)
                    child->siginfo.si_code = 0;
            }
        }
        else
        {
            panic("wait type", "%d", wait->type);
        }

        return info;
    }

    siginfo_t GetChildrenSiginfo(DISPATCHER_ENTRY *entry, bool consume)
    {
        PROCESS *parent = entry->context.thread->process;
        PROCESS *child = parent->first_child;

        while (child)
        {
            siginfo_t info = GetSiginfo(entry, child, consume);

            if (SIGINFO_ANY(info))
                return info;

            child = child->next_sibling;
        }

        siginfo_t info;
        info.si_code = 0;
        return info;
    }

    int SiginfoStatus(const siginfo_t &info)
    {
        int status = 0;
        int code = info.si_code;

        if (code == CLD_EXITED)
        {
            status |= (info.si_status) << 8;
        }
        else if (code == CLD_KILLED)
        {
            status |= (info.si_status) & 0x7f;
        }
        else if (code == CLD_STOPPED)
        {
            status |= 0x7f;
            status |= (info.si_status) << 8;
        }
        else if (code == CLD_CONTINUED)
        {
        }

        return status;
    }

    bool AnyUnwaitedForChildren(PROCESS *parent)
    {
        PROCESS *child = parent->first_child;

        while (child)
        {
            if (child->state != PROCESS_STATE_ZOMBIE || SIGINFO_ANY(child->siginfo))
                return true;

            child = child->next_sibling;
        }

        return false;
    }

    void HandleSignal(PROCESS *process)
    {
        siginfo_t info = process->siginfo;

        if (!info.si_code)
            return;

        Scheduler::Disable();

        for (int i = 0; i < DISPATCHER_THREADS; i++)
        {
            DISPATCHER_ENTRY *entry = &entries[i];
            DISPATCHER_WAIT_INFO *wait = &entry->wait_info;

            if (wait->waiting)
            {
                siginfo_t info = GetSiginfo(entry, process, true);

                if (SIGINFO_ANY(info))
                {
                    wait->siginfo = info;
                    wait->event.Set();
                }
            }
        }

        Scheduler::Enable();
    }

    int Waitpid(pid_t pid, int *status, int options)
    {
        Scheduler::Disable();

        debug_print("Waitpid %d %d\n", pid, options);

        DISPATCHER_ENTRY *entry = Dispatcher::CurrentEntry();
        PROCESS *parent = entry->context.thread->process;
        DISPATCHER_WAIT_INFO *wait = &entry->wait_info;

        if (pid < -1)
        {
            panic("Waitpid pid < -1", "");
        }
        else if (pid == -1)
        {
            if (!AnyUnwaitedForChildren(parent))
            {
                Scheduler::Enable();
                return -ECHILD;
            }

            wait->pid = -pid;
            wait->type = P_ALL;
        }
        else if (pid == 0)
        {
            panic("Waitpid pid = 0", "");
        }
        else
        {
            PROCESS *proc = ProcessManager::GetProcess(pid);

            if (!proc || proc->parent != parent)
            {
                Scheduler::Enable();
                return -ECHILD;
            }

            wait->pid = pid;
            wait->type = P_PID;
        }

        siginfo_t info = GetChildrenSiginfo(entry, true);

        if (SIGINFO_ANY(info))
        {
            if (status)
                *status = SiginfoStatus(info);

            Scheduler::Enable();
            return info.si_pid;
        }
        else
        {
            if (options & WNOHANG)
            {
                if (status)
                    *status = 0;

                Scheduler::Enable();
                return 0;
            }
        }

        wait->waiting = true;
        wait->event.Clear();
        Scheduler::Enable();
        wait->event.Wait();

        if (status)
            *status = SiginfoStatus(wait->siginfo);

        debug_print("Waitpid done\n");
        return wait->siginfo.si_pid;
    }
} // namespace Dispatcher
