#include "vbe.h"
#include "Drawing/drawing.h"
#include "Memory/memory.h"
#include "debug.h"

VBE_MODE_INFO VBE::mode;

uint32* VBE::mem_base;
uint32* VBE::mem_max;
uint32 VBE::mem_size;
uint32 VBE::bytes_per_pixel;
uint32 VBE::pixel_count;

STATUS VBE::Init(VBE_MODE_INFO* info)
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