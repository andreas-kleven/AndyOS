#pragma once
#include "definitions.h"
#include "keycodes.h"
#include "HAL/idt.h"

#define KEYBOARD_IRQ 33
#define MAX_KEYS 0xFF

class Keyboard
{
public:
	static STATUS Init();
	static bool GetLastKey(KEYCODE& code, bool& pressed);

private:
	static void SetupScancodes();
	static void Keyboard_ISR(REGS* regs);
};