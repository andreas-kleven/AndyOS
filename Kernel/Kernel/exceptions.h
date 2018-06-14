#pragma once
#include "definitions.h"
#include "HAL/hal.h"

class Exceptions
{
public:
	static STATUS Init();

	static void ThrowException(char* error, char* msg = "");

	static void DefaultISR(REGS* regs);

private:
	static void ISR0(REGS* regs);
	static void ISR1(REGS* regs);
	static void ISR2(REGS* regs);
	static void ISR3(REGS* regs);
	static void ISR4(REGS* regs);
	static void ISR5(REGS* regs);
	static void ISR6(REGS* regs);
	static void ISR7(REGS* regs);
	static void ISR8(REGS* regs);
	static void ISR9(REGS* regs);
	static void ISR10(REGS* regs);
	static void ISR11(REGS* regs);
	static void ISR12(REGS* regs);
	static void ISR13(REGS* regs);
	static void ISR14(REGS* regs);
	static void ISR15(REGS* regs);
	static void ISR16(REGS* regs);
	static void ISR17(REGS* regs);
	static void ISR18(REGS* regs);
	static void ISR19(REGS* regs);
};