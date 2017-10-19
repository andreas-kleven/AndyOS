#pragma once
#include "Boot/multiboot.h"

static class Kernel
{
public:
	static void Main(MULTIBOOT_INFO* bootinfo);
};