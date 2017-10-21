#include "definitions.h"
#include "multiboot.h"
#include "Memory/paging.h"
#include "Kernel/kernel.h"

__declspec(align(16)) char _kernel_stack[8096];

void Entry(MULTIBOOT_HEADER* header)
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

		//Paging calls kernel
		//Paging::Init(bootinfo);
		Kernel::Setup(bootinfo);
		//Kernel::HigherHalf(bootinfo);
	}

	_asm cli
	_asm hlt
}