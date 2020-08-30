#include <Arch/idt.h>
#include <Arch/scheduler.h>
#include <panic.h>
#include <debug.h>
#include "exceptions.h"

namespace Exceptions
{
	uint32 error_code;

	void ISR0(REGS *regs)
	{
		panic("Division by zero");
	}

	void ISR1(REGS *regs)
	{
		panic("Debug");
	}

	void ISR2(REGS *regs)
	{
		panic("Non-Maskable interrupt");
	}

	void ISR3(REGS *regs)
	{
		panic("Breakpoint");
	}

	void ISR4(REGS *regs)
	{
		panic("Overflow");
	}

	void ISR5(REGS *regs)
	{
		panic("Bound range exceeded");
	}

	void ISR6(REGS *regs)
	{
		debug_pos(0, 1);
		debug_color(0xFFFF0000);

		debug_print("0x%X\t0x%X\t0x%X\t0x%X\n", regs->esp, regs->cs, regs->eip, regs->eflags);
		debug_dump((void *)regs->esp, 256);

		debug_print("Instruction: %p\n", *(uint32*)regs->eip);
		panic("Invalid opcode");
	}

	void ISR7(REGS *regs)
	{
		panic("Device not available");
	}

	void ISR8(REGS *regs)
	{
		panic("Double fault");
	}

	void ISR9(REGS *regs)
	{
		panic("Coprocessor segment overrun");
	}

	void ISR10(REGS *regs)
	{
		panic("Invalid TSS");
	}

	void ISR11(REGS *regs)
	{
		panic("Segment not present");
	}

	void ISR12(REGS *regs)
	{
		panic("Stack-segment fault");
	}

	void ISR13(REGS *regs)
	{
		panic("General protection fault");
	}

	void ISR15(REGS *regs)
	{
		panic("15 (Reserved)");
	}

	void ISR16(REGS *regs)
	{
		panic("x87 Floating-point exception");
	}

	void ISR17(REGS *regs)
	{
		panic("Alignment check");
	}

	void ISR18(REGS *regs)
	{
		panic("Machine check");
	}

	void ISR19(REGS *regs)
	{
		panic("SIMD floating-point exception");
	}

	void INTERRUPT PageFaultISR()
	{
		asm volatile(
			"cli\n"

			//Save error code
			"pop %P0\n"

			//Save registers
			"pusha\n"
			"push %%ds\n"
			"push %%es\n"
			"push %%fs\n"
			"push %%gs\n"

			"mov %%esp, %1\n"

			//Schedule
			"push %2\n"
			"call %P3\n"

			//Load registers
			"mov %4, %%esp\n"

			"pop %%gs\n"
			"pop %%fs\n"
			"pop %%es\n"
			"pop %%ds\n"
			"popa\n"

			"iret"
			: "=m"(error_code), "=m"(Scheduler::Arch::tmp_stack)
			: "N"(14), "i"(&Scheduler::Arch::ScheduleTask), "m"(Scheduler::Arch::tmp_stack));
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
		IDT::SetISR(14, PageFaultISR, 0);
		IDT::InstallIRQ(15, ISR15);
		IDT::InstallIRQ(16, ISR16);
		IDT::InstallIRQ(17, ISR17);
		IDT::InstallIRQ(18, ISR18);
		IDT::InstallIRQ(19, ISR19);

		return true;
	}
} // namespace Exceptions
