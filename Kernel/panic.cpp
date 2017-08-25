#include "panic.h"
#include "vbe.h"

void Panic::KernelPanic(char* err, char* msg)
{
	_asm cli

	VBE::DrawText(0, 0, err, 0xFF0000, 0);

	if (msg)
		VBE::DrawText(0, 16, msg, 0xFF0000, 0);

	VBE::Draw();

	_asm cli
	_asm hlt
}
