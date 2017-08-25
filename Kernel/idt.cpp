#include "idt.h"
#include "string.h"
#include "exceptions.h"
#include "pic.h"

IDT_DESCRIPTOR IDT::idt[MAX_INTERRUPTS];
IDT_REG IDT::idt_reg;

STATUS IDT::Init()
{
	idt_reg.base = (uint32)&idt;
	idt_reg.limit = sizeof(IDT_DESCRIPTOR) * MAX_INTERRUPTS - 1;

	memset(idt, 0, sizeof(IDT_DESCRIPTOR) * MAX_INTERRUPTS - 1);

	for (int i = 0; i < MAX_INTERRUPTS; i++)
		SetISR(i, (IRQ_HANDLER)Exceptions::DefaultISR);

	_asm lidt[idt_reg]

	return STATUS_SUCCESS;
}

STATUS IDT::SetISR(uint32 i, IRQ_HANDLER irq)
{
	if (i > MAX_INTERRUPTS)
		return 0;

	if (!irq)
		return 0;

	uint32 uiBase = (uint32)&(*irq);

	idt[i].low = uint16(uiBase & 0xffff);
	idt[i].high = uint16((uiBase >> 16) & 0xffff);
	idt[i].reserved = 0;
	idt[i].flags = IDT_DESC_PRESENT | IDT_DESC_BIT32;
	idt[i].sel = KERNEL_CS;

	return STATUS_SUCCESS;
}

IDT_DESCRIPTOR* IDT::GetIR(uint32 i)
{
	return &idt[i];
}

void INTERRUPT IDT::EmptyISR()
{
	PIC::InterruptDone(0);
	_asm iretd
}