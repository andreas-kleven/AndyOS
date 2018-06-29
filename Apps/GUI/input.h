#pragma once
#include <AndyOS.h>

#define MAX_KEYS 0xFF

struct KEY_PACKET
{
	KEYCODE code;
	char character;
	bool pressed;
	bool shift;
	bool alt;
	bool ctrl;
	bool caps;
	bool scroll;
	bool numlock;
};

class InputParser
{
public:
    static void Update(KEYCODE code, bool pressed);
    static KEY_PACKET GetPacket();
    static bool GetKeyDown(KEYCODE code);

private:
    static void DecodeCharacter(KEY_PACKET& packet);
};