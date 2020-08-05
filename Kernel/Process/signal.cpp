#include <Process/process.h>
#include <Arch/process.h>
#include <Process/dispatcher.h>
#include <Process/scheduler.h>
#include <sync.h>
#include <errno.h>
#include <debug.h>

namespace ProcessManager
{
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

    int HandleSignal(pid_t sid, gid_t gid, int signo)
    {
        debug_print("Signal group %d %d\n", gid, signo);
        Scheduler::Disable();

        PROCESS *proc = ProcessManager::GetFirst();

        int count = 0;

        while (proc)
        {
            if (proc->sid == sid && proc->gid == gid)
            {
                if (HandleSignal(proc, signo) == 0)
                    count++;
            }

            proc = proc->next;
        }

        Scheduler::Enable();
        return count ? 0 : -1;
    }

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

        ADDRESS_SPACE old_space = VMem::GetAddressSpace();
        VMem::SwapAddressSpace(proc->addr_space);

        if (handler == SIG_DFL)
        {
            Scheduler::Disable();

            if (signo == SIGSTOP)
            {
                if (proc->state == PROCESS_STATE_RUNABLE)
                {
                    proc->siginfo.si_pid = proc->id;
                    proc->siginfo.si_status = signo;
                    proc->siginfo.si_code = CLD_STOPPED;
                    proc->state = PROCESS_STATE_STOPPED;
                    Dispatcher::HandleSignal(proc);

                    if (proc->parent)
                        HandleSignal(proc->parent, SIGCHLD);
                }
            }
            else if (signo == SIGCONT)
            {
                if (proc->state == PROCESS_STATE_STOPPED)
                {
                    proc->siginfo.si_pid = proc->id;
                    proc->siginfo.si_status = signo;
                    proc->siginfo.si_code = CLD_CONTINUED;
                    proc->state = PROCESS_STATE_RUNABLE;
                    Dispatcher::HandleSignal(proc);

                    if (proc->parent)
                        HandleSignal(proc->parent, SIGCHLD);
                }
            }
            else if (signo == SIGCHLD)
            {
                // Ignore
            }
            else
            {
                proc->siginfo.si_pid = proc->id;
                proc->siginfo.si_status = signo;
                proc->siginfo.si_code = CLD_KILLED;
                proc->state = PROCESS_STATE_ZOMBIE;
                Dispatcher::HandleSignal(proc);

                if (proc->parent)
                    HandleSignal(proc->parent, SIGCHLD);

                Terminate(proc);
            }

            Scheduler::Enable();
        }
        else
        {
            proc->signal_mutex.Aquire();
            Arch::HandleSignal(proc, signo, handler);
            proc->signal_mutex.Release();
        }

        VMem::SwapAddressSpace(old_space);
        return 0;
    }
} // namespace ProcessManager
