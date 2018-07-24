#pragma once
#include "definitions.h"
#include "Memory/memory.h"

namespace VMem::Arch
{
	ADDRESS_SPACE GetAddressSpace();
	bool SwapAddressSpace(ADDRESS_SPACE& space);
	ADDRESS_SPACE CreateAddressSpace();

	size_t GetAddress(size_t virt);
	pflags_t GetFlags(size_t virt);

	void* FirstFree(size_t count, size_t start, size_t end);
	bool MapPages(void* virt, void* phys, size_t count, pflags_t flags);
	bool FreePages(void* virt, size_t count);

	bool Init();
}