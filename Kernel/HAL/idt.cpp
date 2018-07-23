#include "idt.h"
#include "hal.h"
#include "string.h"
#include "pic.h"

namespace IDT
{
	IDT_DESCRIPTOR idt[MAX_INTERRUPTS];
	IRQ_HANDLER handlers[MAX_INTERRUPTS];
	IDT_REG idt_reg;

	/*push eax; push irq; mov eax, addr; jmp eax*/
	const uint8 irs_code[] = { 0x50, 0x6A, 0x00, 0xB8, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0 };
	char isrs[MAX_INTERRUPTS * sizeof(irs_code)];

	char fpustate[512];

	IDT_DESCRIPTOR* GetIR(uint32 i)
	{
		return &idt[i];
	}

	void INTERRUPT EmptyISR()
	{
		asm volatile("pusha");
		PIC::InterruptDone(0);
		asm volatile("popa\n"
			"iret");
	}

	void CommonHandler(int i, REGS* regs)
	{
		if (handlers[i])
			handlers[i](regs);

		PIC::InterruptDone(i);
	}

	void INTERRUPT CommonISR()
	{
		asm volatile(
			"cli\n"
			"pop %%eax\n"
			"and $0xFF, %%eax\n"
			"sub $48, %%esp\n"
			"push %%eax\n"
			"add $52, %%esp\n"
			"pop %%eax\n"
			
			"pusha\n"
			"push %%ds\n"
			"push %%es\n"
			"push %%fs\n"
			"push %%gs\n"

			"push %%esp\n"
			"sub $4, %%esp\n"
			"call %P0\n"
			"add $8, %%esp\n"

			"pop %%gs\n"
			"pop %%fs\n"
			"pop %%es\n"
			"pop %%ds\n"
			"popa\n"

			"iret" :: "i" (CommonHandler));
	}

	STATUS SetISR(uint32 i, ISR isr, int flags)
	{
		if (i > MAX_INTERRUPTS || !isr)
			return STATUS_FAILED;

		uint32 uiBase = (uint32)isr;

		idt[i].low = uint16(uiBase & 0xffff);
		idt[i].high = uint16((uiBase >> 16) & 0xffff);
		idt[i].reserved = 0;
		idt[i].flags = IDT_DESC_PRESENT | IDT_DESC_BIT32 | flags;
		idt[i].sel = KERNEL_CS;

		return STATUS_SUCCESS;
	}

	STATUS InstallIRQ(uint32 i, IRQ_HANDLER handler)
	{
		if (i > MAX_INTERRUPTS || !handler)
			return STATUS_FAILED;

		handlers[i] = handler;
		return STATUS_SUCCESS;
	}

	IRQ_HANDLER GetHandler(uint32 i)
	{
		return handlers[i];
	}

	STATUS Init()
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
			uint32 target_addr = (uint32)CommonISR;

			ptr[2] = i;
			ptr[4] = target_addr & 0xFF;
			ptr[5] = (target_addr >> 8) & 0xFF;
			ptr[6] = (target_addr >> 16) & 0xFF;
			ptr[7] = (target_addr >> 24) & 0xFF;

			if (i == 0x80)
				SetISR(i, (ISR)ptr, IDT_DESC_RING3);
			else
				SetISR(i, (ISR)ptr, 0);

			handlers[i] = 0;
		}

		asm volatile("lidt (%0)" :: "r" (&idt_reg));
		return STATUS_SUCCESS;
	}
}

namespace IRQ::Arch
{
	bool Install(int num, IRQ_HANDLER handler)
	{
		return IDT::InstallIRQ(num, handler);
	}
}