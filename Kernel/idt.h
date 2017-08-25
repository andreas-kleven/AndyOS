#pragma once
#include "definitions.h"

#define KERNEL_CS 0x10

#define MAX_INTERRUPTS 256

#define IDT_DESC_BIT16		0x06	//00000110
#define IDT_DESC_BIT32		0x0E	//00001110
#define IDT_DESC_RING1		0x40	//01000000
#define IDT_DESC_RING2		0x20	//00100000
#define IDT_DESC_RING3		0x60	//01100000
#define IDT_DESC_PRESENT	0x80	//10000000

#define INTERRUPT __declspec (naked)
typedef void(_cdecl *IRQ_HANDLER)(void);

struct IDT_DESCRIPTOR
{
	uint16 low;
	uint16 sel;
	uint8 reserved;
	uint8 flags;
	uint16 high;
};

struct IDT_REG
{
	uint16 limit;
	uint32 base;
};

static class IDT
{
public:
	static STATUS Init();
	static STATUS SetISR(uint32 i, IRQ_HANDLER irq);
	static IDT_DESCRIPTOR* GetIR(uint32 i);
	static void _cdecl EmptyISR();

private:
	static IDT_DESCRIPTOR idt[MAX_INTERRUPTS];
	static IDT_REG idt_reg;
};