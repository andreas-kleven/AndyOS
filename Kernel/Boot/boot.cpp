#include "definitions.h"
#include "multiboot.h"
#include "Kernel/kernel.h"
#include "Lib/globals.h"

extern "C" void kernel_main(uint32 magic, MULTIBOOT_INFO* bootinfo)
{
	Kernel::Setup(bootinfo);
}