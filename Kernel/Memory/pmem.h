#pragma once
#include "types.h"

namespace PMem
{
	void InitRegion(void* addr, size_t size);
	void DeInitRegion(void* addr, size_t size);
	void* AllocBlocks(size_t size);
	void FreeBlocks(void* addr, size_t size);
	STATUS Init(size_t mem_start, size_t mem_end, void* map);
};