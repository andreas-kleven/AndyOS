#pragma once
#include "definitions.h"

#define MAX_INTERRUPTS 256

#define IDT_DESC_BIT16		0x06	//00000110
#define IDT_DESC_BIT32		0x0E	//00001110
#define IDT_DESC_RING1		0x40	//01000000
#define IDT_DESC_RING2		0x20	//00100000
#define IDT_DESC_RING3		0x60	//01100000
#define IDT_DESC_PRESENT	0x80	//10000000

#define INTERRUPT __declspec (naked)

struct REGS
{
	uint32 gs, fs, es, ds;
	uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32 eip, cs, eflags;
	uint32 user_stack, user_ss;
};

typedef void(*IRQ_HANDLER)(REGS*);

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
	static STATUS InstallIRQ(uint32 i, IRQ_HANDLER irq);
	static IRQ_HANDLER GetHandler(uint32 i);

private:
	static IDT_DESCRIPTOR idt[MAX_INTERRUPTS];
	static IRQ_HANDLER handlers[MAX_INTERRUPTS];
	static IDT_REG idt_reg;

	static STATUS SetISR(uint32 i, void* irq);
	static IDT_DESCRIPTOR* GetIR(uint32 i);
	static void EmptyISR();
	static void CommonIRQ();
	static void CommonHandler(int i, REGS* regs);
};