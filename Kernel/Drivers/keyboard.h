#pragma once
#include "definitions.h"
#include "keycodes.h"
#include "HAL/idt.h"

#define KEYBOARD_IRQ 33
#define MAX_KEYS 0xFF

struct KEY_PACKET
{
	KEYCODE key;
	char character;
	bool pressed;
	bool shift;
	bool alt;
	bool ctrl;
	bool caps;
	bool scroll;
	bool numlock;
};

class Keyboard
{
public:
	static bool ctrl;
	static bool shift;
	static bool alt;

	static bool caps;
	static bool num;
	static bool scroll;

	static STATUS Init();
	static bool GetLastKey(KEYCODE& code, bool& pressed);
	static bool GetKeyDown(KEYCODE key);

private:
	static void DecodeAscii(KEY_PACKET& key);
	static void Keyboard_ISR(REGS* regs);
};