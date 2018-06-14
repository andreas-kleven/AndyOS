#pragma once
#include "definitions.h"
#include "gdt.h"
#include "tss.h"
#include "pic.h"
#include "idt.h"
#include "pit.h"
#include "rtc.h"

#define KERNEL_CS	0x08
#define KERNEL_SS	0x10
#define KERNEL_DS	0x10

#define USER_CS		0x1B
#define USER_SS		0x23
#define USER_DS		0x23

uint8 inb(uint16 port);
uint16 inw(uint16 port);
uint32 inl(uint16 port);

void outb(uint16 port, uint8 data);
void outw(uint16 port, uint16 data);
void outl(uint16 port, uint32 data);

class HAL
{
public:
	static STATUS Init();
};