#include "HAL/hal.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"

STATUS HAL::Init()
{
	_asm cli

	if (!IDT::Init())
		return STATUS_FAILED;

	if (!PIC::Init())
		return STATUS_FAILED;

	if (!PIT::Init())
		return STATUS_FAILED;

	_asm sti

	return STATUS_SUCCESS;
}


uint8 inb(uint16 port)
{
	uint8 ret;
	_asm
	{
		mov		dx, port
		in		al, dx
		mov		ret, al
	}
	return ret;
}

uint16 inw(uint16 port)
{
	uint16 ret;
	_asm
	{
		mov		dx, port
		in		ax, dx
		mov		ret, ax
	}
	return ret;
}

uint32 inl(uint16 port)
{
	uint32 ret;
	_asm
	{
		mov		dx, port
		in		eax, dx
		mov		ret, eax
	}
	return ret;
}

void outb(uint16 port, uint8 data)
{
	_asm 
	{
		mov		al, data
		mov		dx, port
		out		dx, al
	}
}

void outw(uint16 port, uint16 data)
{
	_asm
	{
		mov		ax, data
		mov		dx, port
		out		dx, ax
	}
}

void outl(uint16 port, uint32 data)
{
	_asm
	{
		mov		eax, data
		mov		dx, port
		out		dx, eax
	}
}
