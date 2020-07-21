#include "Arch/scheduler.h"
#include "Arch/task.h"
#include "Arch/idt.h"
#include "Arch/pit.h"
#include "Arch/tss.h"
#include "Arch/pic.h"
#include "Kernel/task.h"
#include "Process/thread.h"
#include "Process/scheduler.h"
#include "Process/dispatcher.h"
#include "Memory/memory.h"
#include "syscalls.h"
#include "syscall_list.h"
#include "hal.h"
#include "string.h"
#include "debug.h"

namespace Scheduler::Arch
{
    size_t tmp_stack;
    uint8 __attribute__((aligned(16))) fpu_state[512];

    void Switch()
    {
        asm volatile("int %0" ::"N"(SCHEDULE_IRQ));
    }

    void ScheduleTask(bool syscall)
    {
        if (!syscall)
            PIT::ticks++;

        THREAD *current_thread = Scheduler::CurrentThread();

        //Save stack
        current_thread->stack = tmp_stack;

        //Save fpu state
        asm volatile("fxsave (%0)" ::"m"(fpu_state));
        memcpy(current_thread->fpu_state, fpu_state, 512);

        if (syscall)
        {
            REGS *regs = (REGS *)current_thread->stack;

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

        //Schedule
        current_thread = Scheduler::Schedule();

        //Restore fpu state
        memcpy(fpu_state, current_thread->fpu_state, 512);
        asm volatile("fxrstor (%0)"
                     : "=m"(fpu_state));

        //Restore stack
        tmp_stack = current_thread->stack;

        TSS::SetStack(KERNEL_SS, current_thread->kernel_esp);

        if (syscall)
            PIC::InterruptDone(SYSCALL_IRQ);
        else
            PIC::InterruptDone(SCHEDULE_IRQ);
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
            "push $0x0\n"
            "call %P1\n"

            //Load registers
            "mov %2, %%esp\n"

            "pop %%gs\n"
            "pop %%fs\n"
            "pop %%es\n"
            "pop %%ds\n"
            "popa\n"

            "iret"
            : "=m"(tmp_stack)
            : "i"(&ScheduleTask), "m"(tmp_stack));
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
