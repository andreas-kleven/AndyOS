#include <Process/process.h>
#include <Process/scheduler.h>
#include <debug.h>
#include <hal.h>
#include <panic.h>
#include <stdio.h>
#include <string.h>

bool prev_panic = false;

void panic(const char *err, const char *msg_fmt, ...)
{
    disable();

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    va_list args;
    va_start(args, msg_fmt);

    vsnprintf(buffer, sizeof(buffer), msg_fmt, args);

    if (!prev_panic) {
        debug_clear(0xFF000000);
        debug_color(0xFFFF0000);
    }

    kprintf("%s\n", err);

    if (buffer && strlen(buffer) > 0)
        kprintf("%s\n", buffer);

    if (!prev_panic) {
        prev_panic = true;

        THREAD *thread = Scheduler::CurrentThread();

        if (thread->process)
            kprintf("proc:%d thread:%d\n", thread->process->id, thread->id);

        kprintf("-STACK DUMP-\n");
        debug_stackdump(thread, 1024);
    }

    sys_halt();
}
