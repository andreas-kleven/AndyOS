#pragma once
#include "definitions.h"
#include "irq.h"

#define MAX_INTERRUPTS 256

#define IDT_DESC_BIT16		0x06	//00000110
#define IDT_DESC_BIT32		0x0E	//00001110
#define IDT_DESC_RING1		0x40	//01000000
#define IDT_DESC_RING2		0x20	//00100000
#define IDT_DESC_RING3		0x60	//01100000
#define IDT_DESC_PRESENT	0x80	//10000000

#define INTERRUPT __attribute__((naked))

struct IDT_DESCRIPTOR
{
	uint16 low;
	uint16 sel;
	uint8 reserved;
	uint8 flags;
	uint16 high;
} __attribute__((packed));

struct IDT_REG
{
	uint16 limit;
	uint32 base;
} __attribute__((packed));

namespace IDT
{
	STATUS SetISR(uint32 i, ISR isr, int flags);
	STATUS InstallIRQ(uint32 i, IRQ_HANDLER irq);
	IRQ_HANDLER GetHandler(uint32 i);
	STATUS Init();
};

namespace IRQ::Arch
{
	bool Install(int num, IRQ_HANDLER handler);
}