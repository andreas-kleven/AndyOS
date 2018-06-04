#pragma once
#include "definitions.h"

static class Debug
{
public:
	static int x;
	static int y;
	static int x0;

	static uint32 color;
	static uint32 bcolor;

	static STATUS Init(bool serial);
	static void Print(char* str, ...);
	static void Putc(char c, bool escape = true);
	static void Clear(uint32 c);
	
	static void Dump(void* addr, int length, bool str = 0);
};