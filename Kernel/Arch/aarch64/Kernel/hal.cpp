#include "types.h"

void pause()
{
    asm ("yield");
}

void enable()
{
}

void disable()
{
}

void sys_halt()
{
}

//IO
uint8 inb(uint16 port)
{
    return 0;
}

uint16 inw(uint16 port)
{
    return 0;
}

uint32 inl(uint16 port)
{
	return 0;
}

void outb(uint16 port, uint8 val)
{
}

void outw(uint16 port, uint16 val)
{
}

void outl(uint16 port, uint32 val)
{
}