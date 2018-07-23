#pragma once
#include "definitions.h"
#include "gdt.h"
#include "tss.h"
#include "pic.h"
#include "idt.h"
#include "pit.h"

#define KERNEL_CS	0x08
#define KERNEL_SS	0x10
#define KERNEL_DS	0x10

#define USER_CS		0x1B
#define USER_SS		0x23
#define USER_DS		0x23

void pause();
void enable();
void disable();
void halt();

uint8 inb(uint16 port);
uint16 inw(uint16 port);
uint32 inl(uint16 port);

void outb(uint16 port, uint8 val);
void outw(uint16 port, uint16 val);
void outl(uint16 port, uint32 val);

uint8 mmio_read8(uint32 addr);
uint16 mmio_read16(uint32 addr);
uint32 mmio_read32(uint32 addr);

void mmio_write8(uint32 addr, uint8 val);
void mmio_write16(uint32 addr, uint16 val);
void mmio_write32(uint32 addr, uint32 val);

namespace HAL
{
	STATUS Init();
};