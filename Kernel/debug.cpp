#include "debug.h"
#include "string.h"
#include "stdio.h"
#include "drawing.h"
#include "math.h"

int Debug::x;
int Debug::y;

uint32 Debug::color = 0xFFFFFF;
uint32 Debug::bcolor = 0;

void Debug::Print(char* str, ...)
{
	char buffer[256];
	memset(buffer, 0, 256);

	va_list args;
	va_start(args, str);

	str = vsprintf(buffer, str, args);

	while (*str)
		Putc(*str++);

	Drawing::Draw();
}

void Debug::Putc(char c, bool escape)
{
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
			Print("    ");
			break;

		case '\b':
			x = clamp(x - 1, 0, (int)(Drawing::screen.bounds.width / 8));
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

	if (x > Drawing::screen.bounds.width / 8)
	{
		x = 0;
		y++;
	}

	if (y > Drawing::screen.bounds.height / 16)
	{
		x = 0;
		y = 0;
	}
}

void Debug::Clear(uint32 c)
{
	x = 0;
	y = 0;

	Drawing::Clear(c);
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
	Drawing::Draw();
}