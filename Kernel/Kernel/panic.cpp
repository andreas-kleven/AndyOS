#include "panic.h"
#include "string.h"
#include "stdio.h"
#include "Drawing/drawing.h"

void Panic::KernelPanic(char* err, char* msg, ...)
{
	char buffer[256];
	memset(buffer, 0, 256);

	va_list args;
	va_start(args, msg);

	vsprintf(buffer, msg, args);

	_asm cli

	Drawing::DrawText(0, 0, err, 0xFFFF0000, 0xFF000000, Drawing::gc_direct);

	if (msg)
		Drawing::DrawText(0, 16, buffer, 0xFFFF0000, 0xFF000000, Drawing::gc_direct);

	_asm cli
	_asm hlt
}
