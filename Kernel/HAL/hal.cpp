#include "HAL/hal.h"
#include "gdt.h"
#include "tss.h"
#include "pic.h"
#include "idt.h"
#include "pit.h"

STATUS HAL::Init()
{
	asm volatile("cli");

	if (!GDT::Init()) return STATUS_FAILED;
	if (!TSS::Init(5, 0x9000)) return STATUS_FAILED;
	if (!PIC::Init()) return STATUS_FAILED;
	if (!IDT::Init()) return STATUS_FAILED;
	if (!PIT::Init()) return STATUS_FAILED;

	asm volatile("sti");

	return STATUS_SUCCESS;
}

uint8 inb(uint16 port)
{
	uint8 ret;
	asm volatile ("inb %%dx, %%al" : "=a" (ret) : "d" (port));
	return ret;
}

uint16 inw(uint16 port)
{
	uint16 ret;
	asm volatile ("inw %%dx, %%ax" : "=a" (ret) : "d" (port));
	return ret;
}

uint32 inl(uint16 port)
{
	uint32 ret;
	asm volatile ("inl %%dx, %%eax" : "=a" (ret) : "d" (port));
	return ret;
}

void outb(uint16 port, uint8 data)
{
	asm volatile ("outb %%al, %%dx" :: "d" (port), "a" (data));
}

void outw(uint16 port, uint16 data)
{
	asm volatile ("outw %%ax, %%dx" :: "d" (port), "a" (data));
}

void outl(uint16 port, uint32 data)
{
	asm volatile ("outw %%eax, %%dx" :: "d" (port), "a" (data));
}
