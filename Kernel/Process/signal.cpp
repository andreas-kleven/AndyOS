#include <Arch/process.h>
#include <Kernel/timer.h>
#include <Process/dispatcher.h>
#include <Process/process.h>
#include <Process/scheduler.h>
#include <debug.h>
#include <errno.h>
#include <hal.h>
#include <sync.h>

namespace ProcessManager {
sig_t SetSignalHandler(PROCESS *proc, int signo, sig_t handler)
{
    if (!proc)
        return (sig_t)-1;

    if (signo < 0 || signo >= SIGNAL_TABLE_SIZE)
        return (sig_t)-EINVAL;

    sig_t prev = proc->signal_table[signo];

    if (signo == SIGKILL || signo == SIGSTOP)
        proc->signal_table[signo] = SIG_DFL;
    else
        proc->signal_table[signo] = handler;

    return prev;
}

THREAD *InterruptSyscalls(THREAD *thread)
{
    DISPATCHER_ENTRY *dispatcher_entry = Dispatcher::GetEntryFor(thread);

    if (dispatcher_entry) {
        THREAD *handler_thread = dispatcher_entry->handler_thread;
        debug_print("Handler thread %d %d\n", handler_thread->id,
                    dispatcher_entry->context.syscall);

        thread->signal_threads.Push(handler_thread);

        handler_thread->interrupted = true;

        if (handler_thread->sleep_until ||
            (handler_thread->event && handler_thread->event_interruptible)) {
            debug_print("Interrupting handler %d %d %d %d\n", handler_thread->id,
                        handler_thread->state, handler_thread->event, handler_thread->sleep_until);

            if (handler_thread->sleep_until)
                handler_thread->sleep_until = 0;

            if (handler_thread->event && handler_thread->event_interruptible)
                handler_thread->event->Wake(handler_thread, false, true);
        }

        return handler_thread;
    }

    return 0;
}

int HandleSignal(pid_t sid, gid_t gid, int signo)
{
    debug_print("Signal group %d %d\n", gid, signo);

    PROCESS *proc = ProcessManager::GetFirst();

    int count = 0;

    while (proc) {
        if (proc->sid == sid && proc->gid == gid) {
            if (HandleSignal(proc, signo) == 0)
                count++;
        }

        proc = proc->next;
    }

    return count ? 0 : -1;
}

// TODO: refactor
int HandleSignal(PROCESS *proc, int signo)
{
    if (!proc)
        return -1;

    if (signo < 0 || signo >= SIGNAL_TABLE_SIZE)
        return -1;

    sig_t &handler = proc->signal_table[signo];

    debug_print("Signal pid:%d signal:%d handler:%p\n", proc->id, signo, handler);

    if (handler == SIG_IGN)
        return 0;
    if (handler == SIG_ERR)
        return -1;

    if (handler == SIG_DFL) {
        // proc->signal_mutex.Aquire();

        if (signo == SIGSTOP) {
            if (proc->state == PROCESS_STATE_RUNABLE) {
                proc->siginfo.si_pid = proc->id;
                proc->siginfo.si_status = signo;
                proc->siginfo.si_code = CLD_STOPPED;
                proc->state = PROCESS_STATE_STOPPED;

                InterruptSyscalls(proc->main_thread);
                Dispatcher::HandleSignal(proc);

                if (proc->parent)
                    HandleSignal(proc->parent, SIGCHLD);
            }
        } else if (signo == SIGCONT) {
            if (proc->state == PROCESS_STATE_STOPPED) {
                proc->siginfo.si_pid = proc->id;
                proc->siginfo.si_status = signo;
                proc->siginfo.si_code = CLD_CONTINUED;
                proc->state = PROCESS_STATE_RUNABLE;

                InterruptSyscalls(proc->main_thread);
                Dispatcher::HandleSignal(proc);

                if (proc->parent)
                    HandleSignal(proc->parent, SIGCHLD);
            }
        } else if (signo == SIGCHLD) {
            // Ignore
        } else {
            proc->siginfo.si_pid = proc->id;
            proc->siginfo.si_status = signo;
            proc->siginfo.si_code = CLD_KILLED;
            proc->state = PROCESS_STATE_ZOMBIE;

            InterruptSyscalls(proc->main_thread);
            Dispatcher::HandleSignal(proc);

            if (proc->parent)
                HandleSignal(proc->parent, SIGCHLD);

            Terminate(proc);
        }

        // proc->signal_mutex.Release();
    } else {
        ADDRESS_SPACE old_space = VMem::GetAddressSpace();
        VMem::SwapAddressSpace(proc->addr_space);

        THREAD *thread = proc->main_thread;

        disable();
        while (thread->sigret_count > 0) {
            enable();
            Scheduler::Switch();
            disable();
        }
        enable();

        proc->signal_mutex.Aquire();
        Scheduler::Disable();
        disable();

        bool inserted = thread->inserted;

        if (inserted)
            Scheduler::RemoveThread(thread);

        thread->interrupted = true;

        THREAD *handler_thread = InterruptSyscalls(thread);
        thread->signal_threads.Push(handler_thread);

        if (handler_thread) {
            handler_thread->signal_event.Clear();
            handler_thread->signaled = true;
        }

        debug_print("Waiting for syscall to finish...\n");

        Scheduler::Enable();
        enable();
        thread->syscall_event.WaitIntr(); // hack
        disable();
        Scheduler::Disable();

        debug_print("Syscall finished\n");

        Arch::HandleSignal(thread, signo, handler);

        thread->interrupted = false;

        if (inserted)
            Scheduler::InsertThread(thread);

        enable();
        Scheduler::Enable();
        proc->signal_mutex.Release();

        VMem::SwapAddressSpace(old_space);
    }

    return 0;
}

int FinishSignal(THREAD *thread)
{
    debug_print("Finish signal\n");

    PROCESS *proc = thread->process;
    proc->signal_mutex.Aquire();

    if (thread->signal_threads.Count() == 0) {
        panic("FinishSignal", "Signal thread stack empty");
        proc->signal_mutex.Release();
        return -1;
    }

    Scheduler::Disable();

    Arch::FinishSignal(thread);

    THREAD *handler_thread = thread->signal_threads.Pop();

    if (handler_thread) {
        handler_thread->signaled = false;
        handler_thread->signal_event.Set();
    }

    thread->sigret_count -= 1;

    Scheduler::Enable();
    proc->signal_mutex.Release();
    return 0;
}
} // namespace ProcessManager
