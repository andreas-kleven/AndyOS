#include <panic.h>
#include <string.h>
#include <stdio.h>
#include <hal.h>
#include <Process/process.h>
#include <Process/scheduler.h>
#include <debug.h>

bool prev_panic = false;

void panic(const char *err, const char *msg_fmt, ...)
{
	disable();

	char buffer[256];
	memset(buffer, 0, sizeof(buffer));

	va_list args;
	va_start(args, msg_fmt);

	vsnprintf(buffer, sizeof(buffer), msg_fmt, args);

	if (!prev_panic)
	{
		debug_clear(0xFF000000);
		debug_color(0xFFFF0000);
	}

	debug_print("%s\n", err);
	debug_print("%s\n", buffer);

	if (!prev_panic)
	{
		prev_panic = true;

		THREAD *thread = Scheduler::CurrentThread();

		if (thread && thread->process)
			debug_print("proc:%d thread:%d\n", thread->process->id, thread->id);

		debug_print("-STACK DUMP-\n");
		debug_stackdump(thread, 1024);
	}

	sys_halt();
}