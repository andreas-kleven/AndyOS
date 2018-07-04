#include "input.h"
#include "ctype.h"

namespace gui
{
	static bool keystates[MAX_KEYS];
	static KEYCODE last_code;
	static bool last_pressed;

	static int delta_x;
	static int delta_y;

	void InputManager::HandleKey(KEYCODE code, bool pressed)
	{
		if (code == KEY_INVALID || code >= MAX_KEYS)
			return;

		keystates[code] = pressed;
		last_code = code;
		last_pressed = pressed;
	}

	KEY_PACKET InputManager::GetPacket()
	{
		KEY_PACKET pkt;
		pkt.code = last_code;
		pkt.pressed = last_pressed;

		pkt.caps = keystates[KEY_CAPS];
		pkt.scroll = keystates[KEY_SCROLLLOCK];
		pkt.numlock = keystates[KEY_NUMLOCK];

		pkt.shift = keystates[KEY_LSHIFT] | keystates[KEY_RSHIFT];
		pkt.ctrl = keystates[KEY_LCTRL] | keystates[KEY_RCTRL];
		pkt.alt = keystates[KEY_LALT] | keystates[KEY_RALT];

		DecodeCharacter(pkt);
		return pkt;
	}

	bool InputManager::GetKeyDown(KEYCODE code)
	{
		return keystates[code];
	}

	void InputManager::HandleMouse(int dx, int dy)
	{
		delta_x += dx;
		delta_y += dy;
	}

	void InputManager::GetMouseDelta(int& dx, int& dy)
	{
		dx = delta_x;
		dy = delta_y;
	}

	void InputManager::DecodeCharacter(KEY_PACKET& packet)
	{
		uint8 key = packet.code;
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
}