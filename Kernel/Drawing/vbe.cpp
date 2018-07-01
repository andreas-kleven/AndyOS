#include "vbe.h"
#include "font.h"
#include "Memory/memory.h"
#include "string.h"

namespace VBE
{
	VBE_MODE_INFO mode;
	uint32* mem_base;
	uint32* mem_max;
	uint32 mem_size;
	uint32 bytes_per_pixel;
	uint32 pixel_count;

	void Draw(uint32* framebuffer)
	{
		memcpy(mem_base, framebuffer, mem_size);
	}

	void SetPixel(int x, int y, uint32 col)
	{
		if (x >= mode.width || x < 0)
			return;

		if (y >= mode.height || y < 0)
			return;

		uint32* a = mem_base + y * mode.width + x;
		*a = col;
	}

	void DrawText(int x, int y, char* c, uint32 fg, uint32 bg)
	{
		for (int index = 0; index < strlen(c); index++)
		{
			for (int i = 0; i < 16; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					if ((DEFAULT_FONT[i + 16 * c[index]] >> j) & 1)
						SetPixel(x + index * 8 + (8 - j), y + i, fg);
					else
						SetPixel(x + index * 8 + (8 - j), y + i, bg);
				}
			}
		}
	}

	VBE_MODE_INFO GetMode()
	{
		return mode;
	}

	STATUS Init(VBE_MODE_INFO* info)
	{
		mode = *info;

		mem_size = mode.pitch * mode.height;
		bytes_per_pixel = mode.bpp / 8;
		pixel_count = mode.width * mode.height;

		//Map framebuffer
		mem_base = (uint32*)VMem::KernelMapFirstFree(
			(uint32)mode.framebuffer,
			PTE_PRESENT | PTE_WRITABLE,
			BYTES_TO_BLOCKS(mem_size));

		mem_max = mem_base + mem_size;

		return STATUS_SUCCESS;
	}
}