#include "types.h"
#include "multiboot.h"
#include "Kernel/kernel.h"
#include "Drawing/vbe.h"
#include "Lib/globals.h"

extern "C" void grub_main(uint32 magic, MULTIBOOT_INFO* bootinfo)
{
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		return;

	size_t mem_end = bootinfo->mem_upper * 0x400;

	VBE_MODE_INFO* mode_info = (VBE_MODE_INFO*)bootinfo->vbe_mode_info;
	VBEVideoMode video_mode = VBEVideoMode(mode_info);

	Kernel::Setup(0, mem_end, &video_mode);
}