#pragma once
#include "Boot/multiboot.h"

namespace Kernel
{
	void Setup(MULTIBOOT_INFO* bootinfo);
	void HigherHalf(MULTIBOOT_INFO bootinfo);
};