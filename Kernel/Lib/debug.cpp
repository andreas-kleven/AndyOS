#include "Lib/debug.h"
#include "string.h"
#include "stdio.h"
#include "Drawing/drawing.h"
#include "Drivers/serial.h"
#include "math.h"

bool serial;

int Debug::x;
int Debug::y;
int Debug::x0;

uint32 Debug::color = 0xFFFFFFFF;
uint32 Debug::bcolor = 0xFF000000;

STATUS Debug::Init(bool _serial)
{
	serial = _serial;

	if (serial)
		Serial::Init(COM_PORT1, 115200);

	return STATUS_SUCCESS;
}

void Debug::Print(char* str, ...)
{
	char buffer[256];
	memset(buffer, 0, 256);

	va_list args;
	va_start(args, str);

	str = vsprintf(buffer, str, args);

	while (*str)
		Putc(*str++);
}

void Debug::Putc(char c, bool escape)
{
	if (serial)
	{
		Serial::Transmit(COM_PORT1, c);

		if (c == '\n')
			Serial::Transmit(COM_PORT1, '\r');
	}

	if (escape)
	{
		switch (c)
		{
		case '\n':
			x = x0;
			y += 1;
			break;

		case '\r':
			x = x0;
			break;

		case '\t':
			Putc(' ');
			while (x % 4)
				Putc(' ');
			break;

		case '\b':
			x = clamp(x - 1, 0, (int)(Drawing::gc.width / 8));
			Drawing::DrawText(x * 8, y * 16, " ", color, bcolor);
			break;

		default:
			char str[] = { c, '\0' };
			Drawing::DrawText(x * 8, y * 16, str, color, bcolor);
			x++;
			break;
		}
	}
	else
	{
		char str[] = { c, '\0' };
		Drawing::DrawText(x * 8, y * 16, str, color, bcolor);
		x++;
	}

	if (x > Drawing::gc.width / 8)
	{
		x = x0;
		y++;
	}

	if (y > Drawing::gc.height / 16)
	{
		x = x0;
		y = 0;
	}
}

void Debug::Clear(uint32 c)
{
	x = x0;
	y = 0;
}

void Debug::Dump(void* addr, int length, bool str)
{
	uint8* ptr = (uint8*)addr;

	char buf[16];
	int count = 0;

	while (count++ < length)
	{
		int num = *ptr++;

		if (str)
		{
			Debug::Putc(num, 0);
		}
		else
		{
			if (num > 15)
			{
				itoa(num, 16, buf, 0);
			}
			else
			{
				buf[0] = '0';
				itoa(num, 16, buf + 1, 0);
			}

			Debug::Print("%s ", buf);
		}

		if (!str)
		{
			if (count % 32 == 0)
				Putc('\n');
			else if (count % 8 == 0)
				Putc(' ');
		}
	}

	Debug::Putc('\n');;
}