#include "Arch/exec.h"
#include "Arch/regs.h"
#include "string.h"
#include "debug.h"

namespace Exec::Arch
{
    int SetupMain(THREAD *thread, int offset, char const *argv[], char const *envp[])
    {
        int argc = 0;
        char const **arg_ptr = argv;

        while (*arg_ptr++)
            argc++;

        REGS *regs = (REGS *)thread->stack;
        uint32 *ptr = (uint32 *)(regs->user_stack - offset);
        *ptr-- = (uint32)envp;
        *ptr-- = (uint32)argv;
        *ptr-- = argc;

        regs->user_stack = (size_t)ptr;
        return 0;
    }
} // namespace Exec::Arch
