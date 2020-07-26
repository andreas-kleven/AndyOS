#pragma once
#include <types.h>
#include <keycodes.h>

#define KEYBOARD_IRQ 33
#define MAX_KEYS 0xFF

namespace Keyboard
{
	bool GetLastKey(KEYCODE& code, bool& pressed);
	STATUS Init();
};