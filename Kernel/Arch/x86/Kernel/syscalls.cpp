#include <Arch/idt.h>
#include <Arch/scheduler.h>
#include <Kernel/task.h>
#include <Process/scheduler.h>
#include <debug.h>
#include <hal.h>
#include <syscall_list.h>
#include <syscalls.h>

namespace Syscalls::Arch {
int DoSyscall(DISPATCHER_CONTEXT &context, void *location)
{
    uint32 ret;

    asm volatile("push %1\n"
                 "push %2\n"
                 "push %3\n"
                 "push %4\n"
                 "push %5\n"
                 "push %6\n"
                 "call *%7\n"
                 "add $24, %%esp\n"
                 : "=a"(ret)
                 : "m"(context.p6), "m"(context.p5), "m"(context.p4), "m"(context.p3),
                   "m"(context.p2), "m"(context.p1), "m"(location));

    return ret;
}

void ReturnSyscall(DISPATCHER_CONTEXT &context, int value)
{
    THREAD *thread = context.thread;
    REGS *regs = (REGS *)thread->stack;
    regs->eax = value;
}

void INTERRUPT Syscall_ISR()
{
    asm volatile("cli\n"

                 // Save registers
                 "pusha\n"
                 "push %%ds\n"
                 "push %%es\n"
                 "push %%fs\n"
                 "push %%gs\n"

                 "mov %%esp, %0\n"

                 // Schedule
                 "push %1\n"
                 "call %P2\n"

                 // Load registers
                 "mov %3, %%esp\n"

                 "pop %%gs\n"
                 "pop %%fs\n"
                 "pop %%es\n"
                 "pop %%ds\n"
                 "popa\n"

                 "iret"
                 : "=m"(Scheduler::Arch::tmp_stack)
                 : "N"(SYSCALL_IRQ), "i"(&Scheduler::Arch::ScheduleTask),
                   "m"(Scheduler::Arch::tmp_stack));
}

void Init()
{
    IDT::SetISR(SYSCALL_IRQ, Syscall_ISR, IDT_DESC_RING3);
}
} // namespace Syscalls::Arch
