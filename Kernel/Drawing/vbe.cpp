#include "vbe.h"
#include "video.h"
#include "string.h"
#include "Memory/memory.h"

namespace VBE
{
	VBE_MODE_INFO mode;
	uint32* mem_base;
	uint32* mem_max;
	uint32 mem_size;

	STATUS Init(VBE_MODE_INFO* info)
	{
		mode = *info;
		mem_size = mode.pitch * mode.height;

		//Map framebuffer
		mem_base = (uint32*)VMem::KernelMapFirstFree(
			mode.framebuffer, 
			BYTES_TO_BLOCKS(mem_size), 
			PAGE_PRESENT | PAGE_WRITE);

		mem_max = mem_base + mem_size;

		VIDEO_MODE mode(mem_base, info->width, info->height, info->bpp);
    	Video::SetMode(mode);

		return STATUS_SUCCESS;
	}
}