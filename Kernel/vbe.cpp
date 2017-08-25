#include "vbe.h"
#include "string.h"

VBE_MODE_INFO VBE::mode;
uint32* VBE::buffer;

uint32* VBE::mem_base;
uint32* VBE::mem_max;
uint32 VBE::mem_size;
uint32 VBE::bytes_per_pixel;
uint32 VBE::pixel_count;

STATUS VBE::Init(VBE_MODE_INFO* info)
{
	mode = *info;

	mem_base = (uint32*)mode.framebuffer;
	mem_max = mem_base + mode.pitch * mode.height;
	mem_size = mem_max - mem_base;
	bytes_per_pixel = mode.bpp / 8;
	pixel_count = mode.width * mode.height;

	buffer = new uint32[mem_size];

	return STATUS_SUCCESS;
}

void VBE::Draw()
{
	memcpy_fast_128(buffer, (uint32*)mem_base, mem_size);
}

void VBE::Clear(uint32 c)
{
	_asm
	{
		mov edi, buffer
		mov eax, c
		mov ecx, mem_size

		loop_set:
			mov [edi], eax

			add edi, 4
			dec ecx
			jnz loop_set
	}
}

void VBE::SetPixel(uint32 x, uint32 y, uint32 c)
{
	if (x >= mode.width || x < 0)
		return;

	if (y >= mode.height || y < 0)
		return;

	uint32* a = buffer + y * mode.width + x;
	*a = c;
}

void VBE::DrawLine(uint32 x0, uint32 y0, uint32 x1, uint32 y1, uint32 c)
{
	int deltax = x1 - x0;
	int deltay = y1 - y0;

	int y = 0;
	int x = 0;

	int sdx = (deltax < 0) ? -1 : 1;
	int sdy = (deltay < 0) ? -1 : 1;

	deltax = sdx * deltax + 1;
	deltay = sdy * deltay + 1;

	int px = x0;
	int py = y0;

	if (deltax >= deltay)
	{
		for (x = 0; x < deltax; x++)
		{
			if (px < mode.width && px > 0)
				if (py < mode.height && py > 0)
					SetPixel(px, py, c);

			y += deltay;

			if (y >= deltax)
			{
				y -= deltax;
				py += sdy;
			}

			px += sdx;
		}
	}
	else
	{
		for (y = 0; y < deltay; y++)
		{
			if (px < mode.width && px > 0)
				if (py < mode.height && py > 0)
					SetPixel(px, py, c);

			x += deltax;

			if (x >= deltay)
			{
				x -= deltay;
				px += sdx;
			}

			py += sdy;
		}
	}
}

void VBE::DrawText(uint32 x, uint32 y, char* c, uint32 col)
{
	for (int index = 0; index < strlen(c) - 1; index++)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((DEFAULT_FONT[i + 16 * c[index]] >> j) & 1)
					SetPixel(x + index * 8 + (8 - j), y + i, col);
			}
		}
	}
}

void VBE::DrawText(uint32 x, uint32 y, char* c, uint32 col, uint32 bg)
{
	for (int index = 0; index < strlen(c) - 1; index++)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((DEFAULT_FONT[i + 16 * c[index]] >> j) & 1)
					SetPixel(x + index * 8 + (8 - j), y + i, col);
				else
					SetPixel(x + index * 8 + (8 - j), y + i, bg);
			}
		}
	}
}