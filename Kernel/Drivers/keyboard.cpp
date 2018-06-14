#include "keyboard.h"
#include "HAL/hal.h"
#include "string.h"
#include "ctype.h"
#include "Lib/debug.h"

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

bool Keyboard::ctrl;
bool Keyboard::shift;
bool Keyboard::alt;

bool Keyboard::caps;
bool Keyboard::num;
bool Keyboard::scroll;

bool keystates[MAX_KEYS];

bool extended;
uint8 scancode;

KEY_PACKET last_key;

STATUS Keyboard::Init()
{
	IDT::InstallIRQ(KEYBOARD_IRQ, (IRQ_HANDLER)Keyboard_ISR);
	return STATUS_SUCCESS;
}

KEY_PACKET Keyboard::GetLastKey()
{
	return last_key;
}

void Keyboard::DiscardLastKey()
{
	//scancode = 0;
	last_key.key = KEY_INVALID;
}

bool Keyboard::GetKeyDown(KEYCODE key)
{
	return keystates[key];
}

void Keyboard::DecodeAscii(KEY_PACKET& packet)
{
	uint8 key = packet.key;
	packet.character = 0;

	if (packet.ctrl)
		return;

	if (packet.alt)
		return;

	if (!isprint(key))
	{
		switch (key)
		{
		case KEY_SPACE:
			key = ' ';
			break;

		case KEY_TAB:
			key = '\t';
			break;

		case KEY_BACK:
			key = '\b';
			break;

		case KEY_RETURN:
			key = '\n';
			break;

		default:
			return;
		}
	}

	if (isupper(key))
	{
		if (packet.shift == packet.caps)
			key += 32;
	}
	else if (isdigit(key))
	{
		if (packet.shift)
		{
			switch (key)
			{
			case KEY_D0:
				key = '=';
				break;
			case KEY_D1:
				key = '!';
				break;
			case KEY_D2:
				key = '"';
				break;
			case KEY_D3:
				key = '#';
				break;
			case KEY_D4:
				key = '¤';
				break;
			case KEY_D5:
				key = '%';
				break;
			case KEY_D6:
				key = '&';
				break;
			case KEY_D7:
				key = '/';
				break;
			case KEY_D8:
				key = '(';
				break;
			case KEY_D9:
				key = ')';
				break;

			default:
				key = 0;
				break;
			}
		}
	}
	else
	{
		if (packet.shift)
		{
			switch (key)
			{
			case KEY_BAR:
				key = '§';
				break;
			case KEY_PLUS:
				key = '?';
				break;
			case KEY_BACKSLASH:
				key = '`';
				break;
			case KEY_CARET:
				key = '^';
				break;
			case KEY_QUOTE:
				key = '*';
				break;
			case KEY_LESS:
				key = '>';
				break;
			case KEY_COMMA:
				key = ';';
				break;
			case KEY_DOT:
				key = ':';
				break;
			case KEY_MINUS:
				key = '_';
				break;
			}
		}
		else
		{
			switch (key)
			{
			case KEY_BAR:
				key = '|';
				break;
			case KEY_PLUS:
				key = '+';
				break;
			case KEY_BACKSLASH:
				key = '\\';
				break;
			case KEY_CARET:
				key = '^';
				break;
			case KEY_QUOTE:
				key = '\'';
				break;
			case KEY_LESS:
				key = '<';
				break;
			case KEY_COMMA:
				key = ',';
				break;
			case KEY_DOT:
				key = '.';
				break;
			case KEY_MINUS:
				key = '-';
				break;

			case KEY_SPACE:
				key = ' ';
				break;
			case KEY_TAB:
				key = '\t';
				break;
			case KEY_RETURN:
				key = '\n';
				break;
			case KEY_BACK:
				key = '\b';
				break;
			}
		}
	}

	packet.character = key;
}

uint8 scan;
bool pressed;
KEYCODE key;

void Keyboard::Keyboard_ISR(REGS* regs)
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
			if (!extended)
			{
				key = scancodes[scan & ~0x80];
				pressed = !(scan & 0x80);
				keystates[key] = pressed;

				if (pressed)
				{
					if (key == KEY_LCTRL || key == KEY_RCTRL) ctrl = 1;
					else if (key == KEY_LSHIFT || key == KEY_RSHIFT) shift = 1;
					else if (key == KEY_LALT || key == KEY_RALT) alt = 1;

					else if (key == KEY_CAPS) caps = !caps;
					else if (key == KEY_NUMLOCK) num = !num;
					else if (key == KEY_SCROLLLOCK) scroll = !scroll;

					scancode = scan;
				}
				else
				{
					scan -= 0x80;

					if (key == KEY_LCTRL || key == KEY_RCTRL) ctrl = 0;
					else if (key == KEY_LSHIFT || key == KEY_RSHIFT) shift = 0;
					else if (key == KEY_LALT || key == KEY_RALT) alt = 0;
				}

				last_key.key = key;
				last_key.pressed = pressed;
				last_key.shift = shift;
				last_key.alt = alt;
				last_key.ctrl = ctrl;
				last_key.caps = caps;
				last_key.scroll = scroll;
				last_key.numlock = num;
				DecodeAscii(last_key);
				//Set leds
			}

			extended = false;
		}
	}
}
