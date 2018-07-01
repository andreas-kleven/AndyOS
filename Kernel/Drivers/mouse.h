#pragma once
#include "definitions.h"
#include "HAL/idt.h"

#define MOUSE_IRQ 12
#define MOUSE_PORT0 0x60
#define MOUSE_PORT1 0x64

class Mouse
{
public:
	static STATUS Init();

	static void GetButtons(bool& left, bool& right, bool& middle);

	static void GetPos(int& x, int& y);
	static void GetScroll(int& x, int& y);

	static void ResetPos();
	static void ResetScroll();

private:
	static void MouseWait(uint8 type);
	static void MouseWrite(uint8 val);
	static uint8 MouseRead();
	static void Mouse_ISR(REGS* regs);
};