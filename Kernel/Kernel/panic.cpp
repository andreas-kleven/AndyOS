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

	Debug::Clear(0xFF000000);
	Debug::color = 0xFFFF0000;

	Debug::Print("%s\n", err);
	Debug::Print("%s\n", buffer);

	asm volatile(
		"cli\n"
		"hlt");
}
