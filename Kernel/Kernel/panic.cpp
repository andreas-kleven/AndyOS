#include "panic.h"
#include "string.h"
#include "stdio.h"
#include "hal.h"
#include "Process/process.h"
#include "Process/scheduler.h"
#include "Lib/debug.h"

void panic(char* err, char* msg_fmt, ...)
{
	disable();

	char buffer[256];
	memset(buffer, 0, 256);

	va_list args;
	va_start(args, msg_fmt);

	vsprintf(buffer, msg_fmt, args);

	debug_clear(0xFF000000);
	debug_color(0xFFFF0000);

	debug_print("%s\n", err);
	debug_print("%s\n", buffer);

	THREAD* thread = Scheduler::CurrentThread();
	if (thread && thread->process)
		debug_print("Proc: %X\n", Scheduler::CurrentThread()->process->id);

	sys_halt();
}