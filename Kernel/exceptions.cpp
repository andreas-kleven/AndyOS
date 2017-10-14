#include "exceptions.h"
#include "panic.h"
#include "debug.h"

STATUS Exceptions::Init()
{
	IDT::SetISR(0, (IRQ_HANDLER)ISR0);
	IDT::SetISR(1, (IRQ_HANDLER)ISR1);
	IDT::SetISR(2, (IRQ_HANDLER)ISR2);
	IDT::SetISR(3, (IRQ_HANDLER)ISR3);
	IDT::SetISR(4, (IRQ_HANDLER)ISR4);
	IDT::SetISR(5, (IRQ_HANDLER)ISR5);
	IDT::SetISR(6, (IRQ_HANDLER)ISR6);
	IDT::SetISR(7, (IRQ_HANDLER)ISR7);
	IDT::SetISR(8, (IRQ_HANDLER)ISR8);
	IDT::SetISR(9, (IRQ_HANDLER)ISR9);
	IDT::SetISR(10, (IRQ_HANDLER)ISR10);
	IDT::SetISR(11, (IRQ_HANDLER)ISR11);
	IDT::SetISR(12, (IRQ_HANDLER)ISR12);
	IDT::SetISR(13, (IRQ_HANDLER)ISR13);
	IDT::SetISR(14, (IRQ_HANDLER)ISR14);
	IDT::SetISR(15, (IRQ_HANDLER)ISR15);
	IDT::SetISR(16, (IRQ_HANDLER)ISR16);
	IDT::SetISR(17, (IRQ_HANDLER)ISR17);
	IDT::SetISR(18, (IRQ_HANDLER)ISR18);
	IDT::SetISR(19, (IRQ_HANDLER)ISR19);

	return STATUS_SUCCESS;
}

void INTERRUPT Exceptions::DefaultISR()
{
	Panic::KernelPanic("Default interrupt handler");
}

void INTERRUPT Exceptions::ISR0()
{
	Panic::KernelPanic("Division by zero");
}

void INTERRUPT Exceptions::ISR1()
{
	Panic::KernelPanic("Debug");
}

void INTERRUPT Exceptions::ISR2()
{
	Panic::KernelPanic("Non-Maskable interrupt");
}

void INTERRUPT Exceptions::ISR3()
{
	Panic::KernelPanic("Breakpoint");
}
void INTERRUPT Exceptions::ISR4()
{
	Panic::KernelPanic("Overflow");
}

void INTERRUPT Exceptions::ISR5()
{
	Panic::KernelPanic("Bound range exceeded");
}

void INTERRUPT Exceptions::ISR6(uint32 cs, uint32 eip, uint32 eflags)
{
	uint32* stack;

	_asm
	{
		mov eax, esp
		mov stack, eax
	}

	Debug::x = 0;
	Debug::y = 1;
	Debug::color = 0xFFFF0000;

	Debug::Print("0x%ux\t0x%ux\t0x%ux\t0x%ux\n", stack, cs, eip, eflags);
	//Debug::Dump(stack, 256);

	for (int i = 0; i < 32; i++)
	{
		Debug::Print("0x%ux\n", *stack++);
	}

	Panic::KernelPanic("Invalid opcode");
}

void INTERRUPT Exceptions::ISR7()
{
	Panic::KernelPanic("Device not available");
}

void INTERRUPT Exceptions::ISR8()
{
	Panic::KernelPanic("Double fault");
}

void INTERRUPT Exceptions::ISR9()
{
	Panic::KernelPanic("Coprocessor segment overrun");
}

void INTERRUPT Exceptions::ISR10()
{
	Panic::KernelPanic("Invalid TSS");
}

void INTERRUPT Exceptions::ISR11()
{
	Panic::KernelPanic("Segment not present");
}

void INTERRUPT Exceptions::ISR12()
{
	Panic::KernelPanic("Stack-segment fault");
}

void INTERRUPT Exceptions::ISR13()
{
	Panic::KernelPanic("General protection fault");
}

void INTERRUPT Exceptions::ISR14()
{
	Panic::KernelPanic("Page fault");
}

void INTERRUPT Exceptions::ISR15()
{
	Panic::KernelPanic("15 (Reserved)");
}

void INTERRUPT Exceptions::ISR16()
{
	Panic::KernelPanic("x87 Floating-point exception");
}

void INTERRUPT Exceptions::ISR17()
{
	Panic::KernelPanic("Alignment check");
}

void INTERRUPT Exceptions::ISR18()
{
	Panic::KernelPanic("Machine check");
}

void INTERRUPT Exceptions::ISR19()
{
	Panic::KernelPanic("SIMD floating-point exception");
}
