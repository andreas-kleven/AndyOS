#include <Arch/scheduler.h>
#include <Arch/task.h>
#include <Arch/idt.h>
#include <Arch/pit.h>
#include <Arch/tss.h>
#include <Arch/pic.h>
#include <Arch/pagefault.h>
#include <Kernel/task.h>
#include <Process/thread.h>
#include <Process/scheduler.h>
#include <Process/dispatcher.h>
#include <memory.h>
#include <syscalls.h>
#include <syscall_list.h>
#include <hal.h>
#include <string.h>
#include <debug.h>

namespace Scheduler::Arch
{
    size_t tmp_stack;

    void Switch()
    {
        asm volatile("int %0" ::"N"(SCHEDULE_IRQ));
    }

    void ScheduleTask(int irq)
    {
        if (irq == SCHEDULE_IRQ)
            PIT::ticks++;

        THREAD *current_thread = Scheduler::CurrentThread();

        //Save stack
        current_thread->stack = tmp_stack;

        //Save fpu state
        asm volatile("fxsave (%0)" ::"r"(current_thread->fpu_state));

        REGS *regs = (REGS *)current_thread->stack;

        if (irq == SYSCALL_IRQ)
        {
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
        }
        else if (irq == 14)
        {
            PageFault::Arch::HandlePageFault(regs);
        }

        //Schedule
        current_thread = Scheduler::Schedule();

        //Restore fpu state
        asm volatile("fxrstor (%0)" ::"r"(current_thread->fpu_state));

        //Restore stack
        tmp_stack = current_thread->stack;

        TSS::SetStack(KERNEL_SS, current_thread->kernel_esp);
        PIC::InterruptDone(irq);
    }

    void INTERRUPT Schedule_ISR()
    {
        asm volatile(
            "cli\n"

            //Save registers
            "pusha\n"
            "push %%ds\n"
            "push %%es\n"
            "push %%fs\n"
            "push %%gs\n"

            "mov %%esp, %0\n"

            //Schedule
            "push %1\n"
            "call %P2\n"

            //Load registers
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

        asm volatile(
            "mov %0, %%esp\n"
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
