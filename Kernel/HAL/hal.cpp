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
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

uint16 inw(uint16 port)
{
	uint16 ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

uint32 inl(uint16 port)
{
	uint32 ret;
    asm volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

void outb(uint16 port, uint8 val)
{
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void outw(uint16 port, uint16 val)
{
	asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

void outl(uint16 port, uint32 val)
{
	asm volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}
