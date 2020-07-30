#include <debug.h>
#include <Drivers/serial.h>
#include <Drawing/font.h>
#include <video.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

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

void draw_text(int x, int y, const char *c)
{
	for (size_t index = 0; index < strlen(c); index++)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((DEFAULT_FONT[i + 16 * c[index]] >> j) & 1)
					Video::SetPixel(x + index * 8 + (8 - j), y + i, color);
				else
					Video::SetPixel(x + index * 8 + (8 - j), y + i, bcolor);
			}
		}
	}
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

void debug_print(const char *str, ...)
{
	char buffer[256];

	va_list va;
	va_start(va, str);
	int len = vsnprintf(buffer, sizeof(buffer), str, va);
	va_end(va);

	str = buffer;
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

	int width = Video::mode->width;
	int height = Video::mode->height;

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
			draw_text(x * 8, y * 16, " ");
			break;

		default:
			char str[] = {c, '\0'};
			draw_text(x * 8, y * 16, str);
			x++;
			break;
		}
	}
	else
	{
		char str[] = {c, '\0'};
		draw_text(x * 8, y * 16, str);
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

bool debug_dump_row(const uint8 *ptr, int width, bool str)
{
	const int group = 2;

	for (int i = 0; i < width; i++)
	{
		pflags_t flags = VMem::GetFlags((size_t)&ptr[i]);

		if (!(flags & PAGE_PRESENT))
		{
			debug_print("\nMemory dump read error %d\n", flags);
			return false;
		}

		uint8 val = ptr[i];

		if (val > 15)
			debug_print("%x", val);
		else
			debug_print("0%x", val);

		if ((i + 1) % group == 0)
			debug_putc(' ');
	}

	if (str)
	{
		debug_print(" |");

		for (int i = 0; i < width; i++)
		{
			uint8 val = ptr[i];

			if (isprint(val))
				debug_putc(val);
			else
				debug_putc('.');
		}

		debug_putc('|');
	}

	return true;
}

void debug_dump(const void *addr, int length, bool str)
{
	const int width = 32;
	int rows = length / width;

	uint8 *ptr = (uint8 *)addr;

	char buf[16];
	int count = 0;

	for (int i = 0; i <= rows; i++)
	{
		int bytes = min(width, length - i * width);

		if (bytes <= 0)
			break;

		debug_print("%p  ", ptr);

		if (!debug_dump_row(ptr, bytes, str))
			return;

		debug_print("\n");

		ptr += width;
	}

	debug_putc('\n');
}
