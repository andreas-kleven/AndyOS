#include "cpu.h"

STATUS CPU::Init()
{
	if (!EnableFPU())
		return STATUS_FAILED;

	if (!EnableSSE())
		return STATUS_FAILED;

	return STATUS_SUCCESS;
}

STATUS CPU::EnableFPU()
{
	_asm
	{
		_emit 0x0F //mov eax, cr4
		_emit 0x20
		_emit 0xE0

		or eax, 0x200

		_emit 0x0F //mov cr4, eax
		_emit 0x22
		_emit 0xE0
	}

	return STATUS_SUCCESS;
}

STATUS CPU::EnableSSE()
{
	_asm
	{
		mov eax, cr0
		and ax, 0xFFFB
		or ax, 0x2
		mov cr0, eax

		_emit 0x0F //mov eax, cr4
		_emit 0x20
		_emit 0xE0

		or ax, 3 << 9

		_emit 0x0F //mov cr4, eax
		_emit 0x22
		_emit 0xE0
	}

	return STATUS_SUCCESS;
}