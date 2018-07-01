#include "panic.h"
#include "string.h"
#include "stdio.h"
#include "Process/process.h"
#include "Process/scheduler.h"
#include "Lib/debug.h"

void Panic::KernelPanic(char* err, char* msg, ...)
{
	asm volatile("cli");

	char buffer[256];
	memset(buffer, 0, 256);

	va_list args;
	va_start(args, msg);

	vsprintf(buffer, msg, args);

	int line = 0;

	debug_clear(0xFF000000);
	debug_color(0xFFFF0000);

	debug_print("%s\n", err);
	debug_print("%s\n", buffer);

	if (Scheduler::current_thread->process)
		debug_print("Proc: %ux\n", Scheduler::current_thread->process->id);

	asm volatile(
		"cli\n"
		"hlt");
}
