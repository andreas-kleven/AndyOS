#include <Arch/exec.h>
#include <Arch/regs.h>
#include <debug.h>
#include <string.h>

namespace Exec::Arch {
int SetupMain(THREAD *thread, char const *argv[], char const *envp[])
{
    int argc = 0;
    char const **arg_ptr = argv;

    while (*arg_ptr++)
        argc++;

    REGS *regs = (REGS *)thread->stack;
    uint32 *ptr = (uint32 *)(regs->user_stack - 4);
    *ptr-- = (uint32)envp;
    *ptr-- = (uint32)argv;
    *ptr-- = argc;

    regs->user_stack = (size_t)ptr;
    return 0;
}

int SetupThreadMain(THREAD *thread, const void (*start_routine)(), const void *arg)
{
    REGS *regs = (REGS *)thread->stack;
    uint32 *ptr = (uint32 *)(regs->user_stack - 4);
    *ptr-- = (uint32)arg;
    *ptr-- = (uint32)start_routine;

    regs->user_stack = (size_t)ptr;
    return 0;
}
} // namespace Exec::Arch
