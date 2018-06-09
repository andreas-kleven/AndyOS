#include "exceptions.h"
#include "panic.h"
#include "debug.h"

#define PAGE_FAULT_PRESENT				(1 << 0)
#define PAGE_FAULT_WRITE				(1 << 1)
#define PAGE_FAULT_USER					(1 << 2)
#define PAGE_FAULT_RESERVED_WRITE		(1 << 3)
#define PAGE_FAULT_INSTRUCTION_FETCH	(1 << 4)

STATUS Exceptions::Init()
{
	IDT::InstallIRQ(0, (IRQ_HANDLER)ISR0);
	IDT::InstallIRQ(1, (IRQ_HANDLER)ISR1);
	IDT::InstallIRQ(2, (IRQ_HANDLER)ISR2);
	IDT::InstallIRQ(3, (IRQ_HANDLER)ISR3);
	IDT::InstallIRQ(4, (IRQ_HANDLER)ISR4);
	IDT::InstallIRQ(5, (IRQ_HANDLER)ISR5);
	IDT::InstallIRQ(6, (IRQ_HANDLER)ISR6);
	IDT::InstallIRQ(7, (IRQ_HANDLER)ISR7);
	IDT::InstallIRQ(8, (IRQ_HANDLER)ISR8);
	IDT::InstallIRQ(9, (IRQ_HANDLER)ISR9);
	IDT::InstallIRQ(10, (IRQ_HANDLER)ISR10);
	IDT::InstallIRQ(11, (IRQ_HANDLER)ISR11);
	IDT::InstallIRQ(12, (IRQ_HANDLER)ISR12);
	IDT::InstallIRQ(13, (IRQ_HANDLER)ISR13);
	IDT::InstallIRQ(14, (IRQ_HANDLER)ISR14);
	IDT::InstallIRQ(15, (IRQ_HANDLER)ISR15);
	IDT::InstallIRQ(16, (IRQ_HANDLER)ISR16);
	IDT::InstallIRQ(17, (IRQ_HANDLER)ISR17);
	IDT::InstallIRQ(18, (IRQ_HANDLER)ISR18);
	IDT::InstallIRQ(19, (IRQ_HANDLER)ISR19);

	return STATUS_SUCCESS;
}

void Exceptions::ThrowException(char* error, char* msg)
{
	Panic::KernelPanic(error, msg);
}


void Exceptions::DefaultISR(REGS* regs)
{
	Panic::KernelPanic("Default interrupt handler");
}

void Exceptions::ISR0(REGS* regs)
{
	Panic::KernelPanic("Division by zero");
}

void Exceptions::ISR1(REGS* regs)
{
	Panic::KernelPanic("Debug");
}

void Exceptions::ISR2(REGS* regs)
{
	Panic::KernelPanic("Non-Maskable interrupt");
}

void Exceptions::ISR3(REGS* regs)
{
	Panic::KernelPanic("Breakpoint");
}

void Exceptions::ISR4(REGS* regs)
{
	Panic::KernelPanic("Overflow");
}

void Exceptions::ISR5(REGS* regs)
{
	Panic::KernelPanic("Bound range exceeded");
}

void Exceptions::ISR6(REGS* regs)
{
	Debug::x = 0;
	Debug::y = 1;
	Debug::color = 0xFFFF0000;

	Debug::Print("0x%ux\t0x%ux\t0x%ux\t0x%ux\n", regs->esp, regs->cs, regs->eip, regs->eflags);
	Debug::Dump((void*)regs->esp, 256);

	Panic::KernelPanic("Invalid opcode");
}

void Exceptions::ISR7(REGS* regs)
{
	Panic::KernelPanic("Device not available");
}

void Exceptions::ISR8(REGS* regs)
{
	Panic::KernelPanic("Double fault");
}

void Exceptions::ISR9(REGS* regs)
{
	Panic::KernelPanic("Coprocessor segment overrun");
}

void Exceptions::ISR10(REGS* regs)
{
	Panic::KernelPanic("Invalid TSS");
}

void Exceptions::ISR11(REGS* regs)
{
	Panic::KernelPanic("Segment not present");
}

void Exceptions::ISR12(REGS* regs)
{
	Panic::KernelPanic("Stack-segment fault");
}

void Exceptions::ISR13(REGS* regs)
{
	Panic::KernelPanic("General protection fault");
}

void Exceptions::ISR14(REGS* regs)
{
	static int faultAddr = 0;

	_asm
	{
		mov eax, cr2
		mov[faultAddr], eax
	}

	uint32 err = regs->eip;
	regs = (REGS*)((uint32*)regs + 1);

	char* msg1 = (err & PAGE_FAULT_PRESENT) ? "Page-protection violation  " : "None-present page  ";
	char* msg2 = (err & PAGE_FAULT_WRITE) ? "Write  " : "Read  ";
	char* msg3 = (err & PAGE_FAULT_USER) ? "User mode  " : "Kernel mode  ";
	char* msg4 = (err & PAGE_FAULT_RESERVED_WRITE) ? "Reserved write  " : "";
	char* msg5 = (err & PAGE_FAULT_INSTRUCTION_FETCH) ? "Instruction fetch  " : "";

	Panic::KernelPanic("Page fault", "ADDR:%ux  ERR:%ux  EFLAGS:%ux  CS:%ux  EIP:%ux  %s%s%s%s%s",
		faultAddr, err, regs->eflags, regs->cs, regs->eip, msg1, msg2, msg3, msg4, msg5);
}

void Exceptions::ISR15(REGS* regs)
{
	Panic::KernelPanic("15 (Reserved)");
}

void Exceptions::ISR16(REGS* regs)
{
	Panic::KernelPanic("x87 Floating-point exception");
}

void Exceptions::ISR17(REGS* regs)
{
	Panic::KernelPanic("Alignment check");
}

void Exceptions::ISR18(REGS* regs)
{
	Panic::KernelPanic("Machine check");
}

void Exceptions::ISR19(REGS* regs)
{
	Panic::KernelPanic("SIMD floating-point exception");
}
