#pragma once
#include "definitions.h"
#include "HAL/idt.h"

#define KEYBOARD_IRQ 33
#define MAX_KEYS 0xFF

enum KEYCODE
{
	KEY_INVALID,

	KEY_LBUTTON,
	KEY_RBUTTON,
	KEY_MBUTTON,

	KEY_ESCAPE,

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
	KEY_F11,
	KEY_F12,

	KEY_D0 = 0x30,
	KEY_D1,
	KEY_D2,
	KEY_D3,
	KEY_D4,
	KEY_D5,
	KEY_D6,
	KEY_D7,
	KEY_D8,
	KEY_D9,

	KEY_A = 0x41,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_BAR,
	KEY_PLUS,
	KEY_BACKSLASH,
	KEY__1,
	KEY_CARET,
	KEY__2,
	KEY__3,
	KEY_LESS,
	KEY_QUOTE,
	KEY_COMMA,
	KEY_DOT,
	KEY_MINUS,

	KEY_BACK = 0x80,
	KEY_TAB,
	KEY_RETURN,
	KEY_CAPS,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LCTRL,
	KEY_LWIN,
	KEY_LALT,
	KEY_SPACE,
	KEY_RALT,
	KEY_RWIN,
	KEY_MENU,
	KEY_RCTRL,

	KEY_RIGHT,
	KEY_LEFT,
	KEY_UP,
	KEY_DOWN,

	KEY_NUMLOCK,
	KEY_SCROLLLOCK,
};

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
	static KEY_PACKET GetLastKey();
	static void DiscardLastKey();

	static bool GetKeyDown(KEYCODE key);

private:
	static void DecodeAscii(KEY_PACKET& key);
	static void Keyboard_ISR(REGS* regs);
};