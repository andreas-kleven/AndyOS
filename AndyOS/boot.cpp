#include "definitions.h"
#include "kernel.h"
#include "OS.h"

__declspec(align(16)) char _kernel_stack[8096];

void OSBoot(MULTIBOOT_HEADER* header)
{
	uint32 magic;
	MULTIBOOT_INFO* bootinfo;

	__asm
	{
		mov magic, eax
		mov bootinfo, ebx
	}

	if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
	{
		_asm lea esp, WORD ptr[_kernel_stack + 8096];

		Kernel::Init(bootinfo);
		OS::Main();
	}

	_asm cli
	_asm hlt
}