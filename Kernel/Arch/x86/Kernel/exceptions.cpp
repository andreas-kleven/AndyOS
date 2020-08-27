#include <Arch/idt.h>
#include <panic.h>
#include <debug.h>
#include "exceptions.h"

#define PAGE_FAULT_PRESENT				(1 << 0)
#define PAGE_FAULT_WRITE				(1 << 1)
#define PAGE_FAULT_USER					(1 << 2)
#define PAGE_FAULT_RESERVED_WRITE		(1 << 3)
#define PAGE_FAULT_INSTRUCTION_FETCH	(1 << 4)

namespace Exceptions
{
	void ISR0(REGS* regs)
	{
		panic("Division by zero");
	}

	void ISR1(REGS* regs)
	{
		panic("Debug");
	}

	void ISR2(REGS* regs)
	{
		panic("Non-Maskable interrupt");
	}

	void ISR3(REGS* regs)
	{
		panic("Breakpoint");
	}

	void ISR4(REGS* regs)
	{
		panic("Overflow");
	}

	void ISR5(REGS* regs)
	{
		panic("Bound range exceeded");
	}

	void ISR6(REGS* regs)
	{
		debug_pos(0, 1);
		debug_color(0xFFFF0000);

		debug_print("0x%X\t0x%X\t0x%X\t0x%X\n", regs->esp, regs->cs, regs->eip, regs->eflags);
		debug_dump((void*)regs->esp, 256);

		panic("Invalid opcode");
	}

	void ISR7(REGS* regs)
	{
		panic("Device not available");
	}

	void ISR8(REGS* regs)
	{
		panic("Double fault");
	}

	void ISR9(REGS* regs)
	{
		panic("Coprocessor segment overrun");
	}

	void ISR10(REGS* regs)
	{
		panic("Invalid TSS");
	}

	void ISR11(REGS* regs)
	{
		panic("Segment not present");
	}

	void ISR12(REGS* regs)
	{
		panic("Stack-segment fault");
	}

	void ISR13(REGS* regs)
	{
		panic("General protection fault");
	}

	void ISR14(REGS* regs)
	{
		static int faultAddr = 0;

		asm("mov %%cr2, %%eax\n"
			"mov %%eax, (%0)"
			: "=m" (faultAddr));

		uint32 err = regs->eip;
		regs = (REGS*)((uint32*)regs + 1);

		const char* msg1 = (err & PAGE_FAULT_PRESENT) ? "Page-protection violation  " : "None-present page  ";
		const char* msg2 = (err & PAGE_FAULT_WRITE) ? "Write  " : "Read  ";
		const char* msg3 = (err & PAGE_FAULT_USER) ? "User mode  " : "Kernel mode  ";
		const char* msg4 = (err & PAGE_FAULT_RESERVED_WRITE) ? "Reserved write  " : "";
		const char* msg5 = (err & PAGE_FAULT_INSTRUCTION_FETCH) ? "Instruction fetch  " : "";

		if (!(err & PAGE_FAULT_USER))
			regs->user_stack = 0;

		panic("Page fault", "ADDR:%X  ERR:%X  EFLAGS:%X  CS:%X  EIP:%X  ESP:%X  EBP:%X\nEAX:%X  EBX:%X  ECX:%X  EDX:%X  ESI:%X  EDI:%X  %s%s%s%s%s\nUser ESP:%X",
			faultAddr, err, regs->eflags, regs->cs, regs->eip, regs->esp, regs->ebp, regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi, msg1, msg2, msg3, msg4, msg5, regs->user_stack);
	}

	void ISR15(REGS* regs)
	{
		panic("15 (Reserved)");
	}

	void ISR16(REGS* regs)
	{
		panic("x87 Floating-point exception");
	}

	void ISR17(REGS* regs)
	{
		panic("Alignment check");
	}

	void ISR18(REGS* regs)
	{
		panic("Machine check");
	}

	void ISR19(REGS* regs)
	{
		panic("SIMD floating-point exception");
	}

	bool Init()
	{
		IDT::InstallIRQ(0, ISR0);
		IDT::InstallIRQ(1, ISR1);
		IDT::InstallIRQ(2, ISR2);
		IDT::InstallIRQ(3, ISR3);
		IDT::InstallIRQ(4, ISR4);
		IDT::InstallIRQ(5, ISR5);
		IDT::InstallIRQ(6, ISR6);
		IDT::InstallIRQ(7, ISR7);
		IDT::InstallIRQ(8, ISR8);
		IDT::InstallIRQ(9, ISR9);
		IDT::InstallIRQ(10, ISR10);
		IDT::InstallIRQ(11, ISR11);
		IDT::InstallIRQ(12, ISR12);
		IDT::InstallIRQ(13, ISR13);
		IDT::InstallIRQ(14, ISR14);
		IDT::InstallIRQ(15, ISR15);
		IDT::InstallIRQ(16, ISR16);
		IDT::InstallIRQ(17, ISR17);
		IDT::InstallIRQ(18, ISR18);
		IDT::InstallIRQ(19, ISR19);

		return true;
	}
}