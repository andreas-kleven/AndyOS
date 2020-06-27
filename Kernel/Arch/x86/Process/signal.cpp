#include "Arch/process.h"
#include "Arch/regs.h"
#include "Process/thread.h"
#include "Lib/debug.h"

namespace ProcessManager::Arch
{
    void HandleSignal(PROCESS *proc, int signo, sig_t handler)
    {
        THREAD *thread = proc->main_thread;
        REGS *regs = (REGS *)thread->stack;
        uint32 *stack = (uint32 *)regs->user_stack;
        uint32 *code = (uint32 *)proc->heap_start;

        stack[-1] = signo;
        stack[-2] = (uint32)code;
        regs->user_stack -= 8;

        *code++ = 0x6804c483; // add esp, 4
        *code++ = regs->eip;  // push eip
        *code++ = 0xc3;       // ret

        regs->eip = (uint32)handler;
    }
}; // namespace ProcessManager::Arch
