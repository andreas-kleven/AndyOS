#include "types.h"
#include "multiboot.h"
#include "Kernel/kernel.h"
#include "Drawing/vbe.h"
#include "Lib/globals.h"

extern size_t __START_CTORS, __END_CTORS;

void call_ctors()
{
	size_t *ctors = (size_t *)&__START_CTORS;
	int ctor_count = ((size_t)&__END_CTORS - (size_t)&__START_CTORS) / sizeof(size_t);

	for (int i = 1; i < ctor_count - 1; i++)
	{
		void (*func)(void) = (void (*)(void))ctors[i];
		func();
	}
}

extern "C" void grub_main(uint32 magic, MULTIBOOT_INFO *bootinfo)
{
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		return;

	call_ctors();

	size_t mem_end = bootinfo->mem_upper * 0x400;

	VBE_MODE_INFO *mode_info = (VBE_MODE_INFO *)bootinfo->vbe_mode_info;
	VBEVideoMode video_mode = VBEVideoMode(mode_info);

	Kernel::Setup(0, mem_end, &video_mode);
}
