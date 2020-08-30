#include <Arch/idt.h>
#include <Arch/regs.h>
#include <Arch/task.h>
#include <Process/thread.h>
#include <debug.h>
#include <memory.h>
#include <string.h>
#include <sync.h>

namespace Task::Arch {
const int stack_size = PAGE_SIZE * 4;

THREAD *ResetKernelThread(THREAD *thread, void (*entry)())
{
    if (!thread)
        return 0;

    memset(thread, 0, sizeof(THREAD));

    thread->stack = (size_t)thread - sizeof(THREAD) - sizeof(REGS) - 1;
    thread->state = THREAD_STATE_INITIALIZED;
    thread->signal_event = Event(true);
    thread->syscall_event = Event(true);

    asm volatile("fxsave (%0)" ::"r"(thread->fpu_state));

    REGS *regs = (REGS *)thread->stack;
    regs->ebp = 0;
    regs->edi = 0;
    regs->esi = 0;
    regs->edx = 0;
    regs->ecx = 0;
    regs->ebx = 0;
    regs->eax = 0;
    regs->user_stack = 0;
    regs->user_ss = 0;
    regs->esp = thread->stack;
    regs->eip = (uint32)entry;
    regs->eflags = 0x200;
    regs->cs = KERNEL_CS;
    regs->ds = KERNEL_SS;
    regs->es = KERNEL_SS;
    regs->fs = KERNEL_SS;
    regs->gs = KERNEL_SS;

    return thread;
}

THREAD *CreateKernelThread(void (*entry)())
{
    THREAD *thread = (THREAD *)(new char[stack_size] + stack_size - sizeof(THREAD));
    return ResetKernelThread(thread, entry);
}

THREAD *CreateUserThread(void (*entry)(), void *stack)
{
    THREAD *thread = CreateKernelThread(entry);
    thread->kernel_esp = thread->stack;

    REGS *regs = (REGS *)thread->stack;
    regs->cs = USER_CS;
    regs->ds = USER_SS;
    regs->es = USER_SS;
    regs->fs = USER_SS;
    regs->gs = USER_SS;
    regs->user_stack = (uint32)stack;
    regs->user_ss = USER_SS;

    return thread;
}

THREAD *CopyThread(THREAD *old)
{
    char *old_stack = (char *)old - stack_size + sizeof(THREAD);
    char *new_stack = new char[stack_size];
    memcpy(new_stack, old_stack, stack_size);

    ssize_t offset = (ssize_t)new_stack - (ssize_t)old_stack;
    THREAD *thread = (THREAD *)((size_t)old + offset);
    thread->stack = old->stack + offset;

    if (thread->kernel_esp)
        thread->kernel_esp = old->kernel_esp + offset;

    return thread;
}

int SetThreadReturn(THREAD *thread, int ret)
{
    REGS *regs = (REGS *)thread->stack;
    regs->eax = ret;
    return 1;
}
} // namespace Task::Arch
