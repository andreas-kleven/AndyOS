#pragma once
#include "definitions.h"
#include "idt.h"

static class Exceptions
{
public:
	static STATUS Init();
	static void _cdecl DefaultISR();

private:
	static void _cdecl ISR0();
	static void _cdecl ISR1();
	static void _cdecl ISR2();
	static void _cdecl ISR3();
	static void _cdecl ISR4();
	static void _cdecl ISR5();
	static void _cdecl ISR6(uint32 cs, uint32 eip, uint32 eflags);
	static void _cdecl ISR7();
	static void _cdecl ISR8();
	static void _cdecl ISR9();
	static void _cdecl ISR10();
	static void _cdecl ISR11();
	static void _cdecl ISR12();
	static void _cdecl ISR13();
	static void _cdecl ISR14();
	static void _cdecl ISR15();
	static void _cdecl ISR16();
	static void _cdecl ISR17();
	static void _cdecl ISR18();
	static void _cdecl ISR19();
};