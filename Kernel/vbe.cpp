#include "vbe.h"
#include "drawing.h"

VBE_MODE_INFO VBE::mode;

uint32* mem_base;
uint32* mem_max;
uint32 mem_size;
uint32 bytes_per_pixel;
uint32 pixel_count;

STATUS VBE::Init(VBE_MODE_INFO* info)
{
	mode = *info;

	mem_base = (uint32*)mode.framebuffer;
	mem_max = mem_base + mode.pitch * mode.height;
	mem_size = mem_max - mem_base;
	bytes_per_pixel = mode.bpp / 8;
	pixel_count = mode.width * mode.height;

	return STATUS_SUCCESS;
}