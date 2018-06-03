#include "idt.h"
#include "hal.h"
#include "string.h"
#include "pic.h"
#include "debug.h"

IDT_DESCRIPTOR IDT::idt[MAX_INTERRUPTS];
IRQ_HANDLER IDT::handlers[MAX_INTERRUPTS];
IDT_REG IDT::idt_reg;

/*push eax; push irq; mov eax, addr; jmp eax*/
const char irs_code[] = { 0x50, 0x6A, 0x00, 0xB8, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0 };
char isrs[MAX_INTERRUPTS * sizeof(irs_code)];

char fpustate[512];

STATUS IDT::Init()
{
	idt_reg.base = (uint32)&idt;
	idt_reg.limit = sizeof(IDT_DESCRIPTOR) * MAX_INTERRUPTS - 1;

	memset(idt, 0, sizeof(IDT_DESCRIPTOR) * MAX_INTERRUPTS - 1);

	for (int i = 0; i < MAX_INTERRUPTS; i++)
	{
		//Copy irq code
		char* ptr = (char*)&isrs[i * sizeof(irs_code)];
		memcpy(ptr, (char*)irs_code, sizeof(irs_code));

		//Set irq values
		uint32 target_addr = (uint32)CommonIRQ;

		ptr[2] = i;
		ptr[4] = target_addr & 0xFF;
		ptr[5] = (target_addr >> 8) & 0xFF;
		ptr[6] = (target_addr >> 16) & 0xFF;
		ptr[7] = (target_addr >> 24) & 0xFF;

		if (i == 0x80)
			SetISR(i, ptr, IDT_DESC_RING3);
		else
			SetISR(i, ptr, 0);

		handlers[i] = 0;
	}

	_asm lidt[idt_reg]

		return STATUS_SUCCESS;
}

STATUS IDT::SetISR(uint32 i, void* irq, int flags)
{
	if (i > MAX_INTERRUPTS || !irq)
		return STATUS_FAILED;

	uint32 uiBase = (uint32)irq;

	idt[i].low = uint16(uiBase & 0xffff);
	idt[i].high = uint16((uiBase >> 16) & 0xffff);
	idt[i].reserved = 0;
	idt[i].flags = IDT_DESC_PRESENT | IDT_DESC_BIT32 | flags;
	idt[i].sel = KERNEL_CS;

	return STATUS_SUCCESS;
}

STATUS IDT::InstallIRQ(uint32 i, IRQ_HANDLER handler)
{
	if (i > MAX_INTERRUPTS || !handler)
		return STATUS_FAILED;

	handlers[i] = handler;
	return STATUS_SUCCESS;
}

IRQ_HANDLER IDT::GetHandler(uint32 i)
{
	return handlers[i];
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

void INTERRUPT IDT::CommonIRQ()
{
	_asm
	{
		//Pop irq and eax
		pop eax
		and eax, 0xFF
		sub esp, 48
		push eax
		add esp, 52
		pop eax

		//Push registers
		pushad

		push ds
		push es
		push fs
		push gs

		fxsave fpustate

		//Call handler
		push esp
		sub esp, 4
		call CommonHandler
		add esp, 8

		fxrstor fpustate

		//Pop registers
		pop gs
		pop fs
		pop es
		pop ds

		popad

		//Return
		iretd
	}
}

void IDT::CommonHandler(int i, REGS* regs)
{
	if (handlers[i])
		handlers[i](regs);

	PIC::InterruptDone(i);
}