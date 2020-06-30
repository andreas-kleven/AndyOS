#include "process.h"
#include "Arch/process.h"
#include "sync.h"
#include "debug.h"

namespace ProcessManager
{
    int SetSignalHandler(PROCESS *proc, int signo, sig_t handler)
    {
        if (!proc)
            return -1;

        if (signo < 0 || signo >= SIGNAL_TABLE_SIZE)
            return -1;

        if (signo == SIGKILL || signo == SIGSTOP)
            proc->signal_table[signo] = SIG_DFL;
        else
            proc->signal_table[signo] = handler;
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
            Terminate(proc);
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
