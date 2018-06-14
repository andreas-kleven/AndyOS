#include "definitions.h"
#include "multiboot.h"
#include "Kernel/kernel.h"
#include "Lib/globals.h"

char __attribute__((aligned(16))) kernel_stack[8096];

extern "C" void kernel_main(uint32 magic, MULTIBOOT_INFO* bootinfo)
{
	asm volatile("mov %0, %%esp" :: "r" (&kernel_stack));
	Kernel::Setup(bootinfo);
}