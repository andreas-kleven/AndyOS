#pragma once
#include "definitions.h"

namespace PMem
{
	STATUS Init(uint32 size, uint32* map);
	void InitRegion(void* addr, uint32 size);
	void DeInitRegion(void* addr, uint32 size);
	void* AllocBlocks(uint32 size);
	void FreeBlocks(void* addr, uint32 size);
};