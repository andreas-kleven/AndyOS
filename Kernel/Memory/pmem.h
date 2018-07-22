#pragma once
#include "definitions.h"

namespace PMem
{
	STATUS Init(size_t size, void* map);
	void InitRegion(void* addr, size_t size);
	void DeInitRegion(void* addr, size_t size);
	void* AllocBlocks(size_t size);
	void FreeBlocks(void* addr, size_t size);
};