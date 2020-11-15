#include <Arch/idt.h>
#include <Arch/scheduler.h>
#include <Kernel/task.h>
#include <Process/scheduler.h>
#include <debug.h>
#include <hal.h>
#include <syscall_list.h>
#include <syscalls.h>

namespace Syscalls::Arch {
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
                 "and $0xFFFFFFF0, %%esp\n"
                 "sub $12, %%esp\n"
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
