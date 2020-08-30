#pragma once
#include <Kernel/task.h>
#include <types.h>

#define SCHEDULE_IRQ 32

namespace Scheduler::Arch {
extern size_t tmp_stack;

void ScheduleTask(int irq);
void Start(THREAD *thread);
void Switch();
void Idle();
}; // namespace Scheduler::Arch
