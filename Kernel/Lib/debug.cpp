#include "debug.h"
#include "Drivers/serial.h"
#include "Drawing/vbe.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

static bool serial;
static int x;
static int y;

static uint32 color = 0xFFFFFFFF;
static uint32 bcolor = 0xFF000000;

STATUS debug_init(bool _serial)
{
	serial = _serial;

	if (serial)
		Serial::Init(COM_PORT1, 115200);

	return STATUS_SUCCESS;
}

void debug_pos(int _x, int _y)
{
	x = _x;
	y = _y;
}

void debug_color(uint32 foreground, uint32 background)
{
	color = foreground;
	bcolor = background;
}

void debug_print(char* str, ...)
{
	char buffer[256];
	memset(buffer, 0, 256);

	va_list args;
	va_start(args, str);

	str = vsprintf(buffer, str, args);

	while (*str)
		debug_putc(*str++);
}

void debug_putc(char c, bool escape)
{
	if (serial)
	{
		Serial::Transmit(COM_PORT1, c);

		if (c == '\n')
			Serial::Transmit(COM_PORT1, '\r');
	}

	VBE_MODE_INFO mode = VBE::GetMode();
	int width = mode.width;
	int height = mode.height;

	if (escape)
	{
		switch (c)
		{
		case '\n':
			x = 0;
			y += 1;
			break;

		case '\r':
			x = 0;
			break;

		case '\t':
			debug_putc(' ');
			while (x % 4)
				debug_putc(' ');
			break;

		case '\b':
			x = clamp(x - 1, 0, width / 8);
			VBE::DrawText(x * 8, y * 16, " ", color, bcolor);
			break;

		default:
			char str[] = { c, '\0' };
			VBE::DrawText(x * 8, y * 16, str, color, bcolor);
			x++;
			break;
		}
	}
	else
	{
		char str[] = { c, '\0' };
		VBE::DrawText(x * 8, y * 16, str, color, bcolor);
		x++;
	}

	if (x > width / 8)
	{
		x = 0;
		y++;
	}

	if (y > height / 16)
	{
		x = 0;
		y = 0;
	}
}

void debug_clear(uint32 c)
{
	x = 0;
	y = 0;
}

void debug_dump(void* addr, int length, bool str)
{
	uint8* ptr = (uint8*)addr;

	char buf[16];
	int count = 0;

	while (count++ < length)
	{
		int num = *ptr++;

		if (str)
		{
			debug_putc(num, 0);
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

			debug_print("%s ", buf);
		}

		if (!str)
		{
			if (count % 32 == 0)
				debug_putc('\n');
			else if (count % 8 == 0)
				debug_putc(' ');
		}
	}

	debug_putc('\n');;
}