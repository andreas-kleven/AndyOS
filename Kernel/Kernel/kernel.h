#pragma once
#include "Boot/multiboot.h"

static class Kernel
{
public:
	static void Setup(MULTIBOOT_INFO* bootinfo);
	static void HigherHalf(MULTIBOOT_INFO* bootinfo);
};