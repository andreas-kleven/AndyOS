#pragma once
#include "definitions.h"
#include "keycodes.h"
#include "HAL/idt.h"

#define KEYBOARD_IRQ 33
#define MAX_KEYS 0xFF

namespace Keyboard
{
	bool GetLastKey(KEYCODE& code, bool& pressed);
	STATUS Init();
};