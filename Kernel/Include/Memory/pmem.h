#pragma once
#include <types.h>

namespace PMem {
size_t NumBlocks();
size_t NumFree();
size_t NumUsed();

void InitRegion(size_t addr, size_t size);
void DeInitRegion(size_t addr, size_t size);
void *AllocBlocks(size_t size);
void FreeBlocks(void *addr, size_t size);
STATUS Init(size_t size, void *map);
}; // namespace PMem
