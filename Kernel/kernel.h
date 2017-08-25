#pragma once
#include "multiboot.h"

static class Kernel
{
public:
	static void Init(MULTIBOOT_INFO* bootinfo);
};