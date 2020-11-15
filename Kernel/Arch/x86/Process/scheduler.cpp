#include <Arch/fpu.h>
#include <Arch/idt.h>
#include <Arch/pagefault.h>
#include <Arch/pic.h>
#include <Arch/pit.h>
#include <Arch/scheduler.h>
#include <Arch/task.h>
#include <Arch/tss.h>
#include <Kernel/task.h>
#include <Process/dispatcher.h>
#include <Process/scheduler.h>
#include <Process/thread.h>
#include <debug.h>
#include <hal.h>
#include <memory.h>
#include <string.h>
#include <syscall_list.h>
#include <syscalls.h>

namespace Scheduler::Arch {
size_t tmp_stack;
bool switched = false;

void Switch()
{
    switched = true;
    asm volatile("int %0" ::"N"(SCHEDULE_IRQ));
}

void ScheduleTask(int irq)
{
    FPU::SetTS();
    Scheduler::InterruptEnter();

    if (irq == SCHEDULE_IRQ && !switched)
        PIT::ticks++;

    switched = false;

    THREAD *current_thread = Scheduler::CurrentThread();

    // Save stack
    current_thread->stack = tmp_stack;

    REGS *regs = (REGS *)current_thread->stack;

    if (irq == SYSCALL_IRQ) {
        DISPATCHER_CONTEXT context;
        context.thread = current_thread;
        context.syscall = regs->eax;
        context.p1 = regs->ebx;
        context.p2 = regs->ecx;
        context.p3 = regs->edx;
        context.p4 = regs->esi;
        context.p5 = regs->edi;
        context.p6 = regs->ebp;

        Dispatcher::Dispatch(context);
        Scheduler::BlockThread(current_thread, false);
    } else if (irq == 14) {
        PageFault::Arch::HandlePageFault(regs);
    }

    // Schedule
    current_thread = Scheduler::Schedule();

    // Restore stack
    tmp_stack = current_thread->stack;

    TSS::SetStack(KERNEL_SS, current_thread->kernel_esp);
    PIC::InterruptDone(irq);

    Scheduler::InterruptExit();
    FPU::SetTS();
}

void INTERRUPT Schedule_ISR()
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
                 : "=m"(tmp_stack)
                 : "N"(SCHEDULE_IRQ), "i"(&ScheduleTask), "m"(tmp_stack));
}

void Start(THREAD *thread)
{
    disable();
    IDT::SetISR(SCHEDULE_IRQ, Schedule_ISR, 0);

    FPU::SetTS();

    asm volatile("mov %0, %%esp\n"
                 "pop %%gs\n"
                 "pop %%fs\n"
                 "pop %%es\n"
                 "pop %%ds\n"
                 "popa\n"
                 "iret" ::"r"(thread->stack));
}

void Idle()
{
    while (1)
        asm volatile("hlt");
}
} // namespace Scheduler::Arch
