#include <Arch/fpu.h>
#include <Arch/scheduler.h>
#include <Process/scheduler.h>
#include <debug.h>

namespace NMFault::Arch {

THREAD *prev_nmthread = 0;

void HandleNMFault()
{
    FPU::ClearTS();

    if (prev_nmthread)
        asm volatile("fxsave (%0)" ::"r"(&prev_nmthread->fpu_state));

    if (Scheduler::IsInterrupt()) {
        prev_nmthread = 0;
        asm volatile("finit");
    } else {
        THREAD *thread = Scheduler::CurrentThread();

        if (thread != prev_nmthread) {
            prev_nmthread = thread;
            asm volatile("fxrstor (%0)" ::"r"(thread->fpu_state));
        }
    }
}

void Trigger()
{
    asm volatile("int %0" ::"N"(7));
}

void Init()
{
    prev_nmthread = Scheduler::CurrentThread();
}

} // namespace NMFault::Arch
