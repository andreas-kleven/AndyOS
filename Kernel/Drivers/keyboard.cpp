#include "keyboard.h"
#include "HAL/hal.h"
#include "io.h"
#include "irq.h"
#include "string.h"
#include "ctype.h"
#include "Lib/circbuf.h"
#include "Lib/debug.h"

#define KEY_BUFFER_SIZE 32

namespace Keyboard
{
	struct KEY_ACTION
	{
		KEYCODE code;
		bool pressed;
	};

	const KEYCODE scancodes[] =
	{
		KEY_INVALID,
		KEY_ESCAPE,
		KEY_D1,
		KEY_D2,
		KEY_D3,
		KEY_D4,
		KEY_D5,
		KEY_D6,
		KEY_D7,
		KEY_D8,
		KEY_D9,
		KEY_D0,
		KEY_PLUS,
		KEY_BACKSLASH,
		KEY_BACK,
		KEY_TAB,
		KEY_Q,
		KEY_W,
		KEY_E,
		KEY_R,
		KEY_T,
		KEY_Y,
		KEY_U,
		KEY_I,
		KEY_O,
		KEY_P,
		KEY__1,
		KEY_CARET,
		KEY_RETURN,
		KEY_LCTRL,
		KEY_A,
		KEY_S,
		KEY_D,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY__2,
		KEY__3,
		KEY_BAR,
		KEY_LSHIFT,
		KEY_QUOTE,
		KEY_Z,
		KEY_X,
		KEY_C,
		KEY_V,
		KEY_B,
		KEY_N,
		KEY_M,
		KEY_COMMA,
		KEY_DOT,
		KEY_MINUS,
		KEY_RSHIFT,
		KEY_INVALID,
		KEY_LALT,
		KEY_SPACE,
		KEY_CAPS,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_INVALID,
		KEY_SCROLLLOCK,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
		KEY_LESS,
		KEY_F11,
		KEY_F12,
		KEY_INVALID,
		KEY_INVALID,
		KEY_INVALID,
	};

	KEYCODE extended_scancodes[256];

	bool ctrl;
	bool shift;
	bool alt;

	bool caps;
	bool num;
	bool scroll;

	bool extended;
	uint8 scancode;

	uint8 scan;
	bool pressed;
	KEYCODE code;

	CircularBuffer<KEY_ACTION> key_buffer;

	void Keyboard_ISR()
	{
		if (inb(0x64) & 1)
		{
			scan = inb(0x60);

			if (scan == 0xE0 || scan == 0xE1)
			{
				extended = true;
			}
			else
			{
				if (extended)
				{
					extended = false;
					code = extended_scancodes[scan & ~0x80];
				}
				else
				{
					code = scancodes[scan & ~0x80];
				}

				pressed = !(scan & 0x80);

				if (pressed)
				{
					if (code == KEY_LCTRL || code == KEY_RCTRL) ctrl = 1;
					else if (code == KEY_LSHIFT || code == KEY_RSHIFT) shift = 1;
					else if (code == KEY_LALT || code == KEY_RALT) alt = 1;

					else if (code == KEY_CAPS) caps = !caps;
					else if (code == KEY_NUMLOCK) num = !num;
					else if (code == KEY_SCROLLLOCK) scroll = !scroll;

					scancode = scan;
				}
				else
				{
					scan -= 0x80;

					if (code == KEY_LCTRL || code == KEY_RCTRL) ctrl = 0;
					else if (code == KEY_LSHIFT || code == KEY_RSHIFT) shift = 0;
					else if (code == KEY_LALT || code == KEY_RALT) alt = 0;
				}

				KEY_ACTION key;
				key.code = code;
				key.pressed = pressed;

				key_buffer.Add(key);
			}
		}
	}

	void SetupScancodes()
	{
		extended_scancodes[0x48] = KEY_UP;
		extended_scancodes[0x49] = KEY_PAGEUP;
		extended_scancodes[0x4B] = KEY_LEFT;
		extended_scancodes[0x4D] = KEY_RIGHT;
		extended_scancodes[0x50] = KEY_DOWN;
		extended_scancodes[0x51] = KEY_PAGEDOWN;
	}

	bool GetLastKey(KEYCODE& code, bool& pressed)
	{
		if (key_buffer.IsEmpty())
			return false;

		KEY_ACTION* packet = key_buffer.Get();

		if (!packet)
			return false;

		code = packet->code;
		pressed = packet->pressed;

		if (code == KEY_INVALID)
			return false;

		return true;
	}

	STATUS Init()
	{
		key_buffer = CircularBuffer<KEY_ACTION>(KEY_BUFFER_SIZE);
		SetupScancodes();
		IRQ::Install(KEYBOARD_IRQ, Keyboard_ISR);
		return STATUS_SUCCESS;
	}
}