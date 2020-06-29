#pragma once
#include "types.h"
#include "Kernel/task.h"

#define SCHEDULE_IRQ 32

namespace Scheduler::Arch
{
    extern size_t tmp_stack;

    void ScheduleTask(bool syscall);
    void Start(THREAD *thread);
    void Switch();
    void Idle();
}; // namespace Scheduler::Arch
