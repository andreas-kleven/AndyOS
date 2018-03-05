#pragma once
#include "definitions.h"

#define MOUSE_IRQ 12
#define MOUSE_PORT0 0x60
#define MOUSE_PORT1 0x64

static class Mouse
{
public:
	static float x;
	static float y;
	static float sensitivity;

	static bool mouse_L;
	static bool mouse_R;
	static bool mouse_M;

	static int scroll_x;
	static int scroll_y;

	static STATUS Init(uint32 width, uint32 height, float sens);

private:
	static uint32 w;
	static uint32 h;

	static uint8 mouse_cycle;
	static int8 mouse_byte[];

	static void MouseWait(uint8 type);
	static void MouseWrite(uint8 val);
	static uint8 MouseRead();
	static void _cdecl Mouse_ISR();

	static bool initialized;
};