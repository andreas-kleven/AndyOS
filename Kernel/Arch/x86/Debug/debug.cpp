#include <Arch/regs.h>
#include <debug.h>
#include <memory.h>

void debug_stackdump(THREAD *thread, int length)
{
    REGS *regs = (REGS *)thread->stack;
    size_t addr = regs->user_stack ? regs->user_stack : regs->esp;
    debug_dump((void *)addr, length, true);
}
