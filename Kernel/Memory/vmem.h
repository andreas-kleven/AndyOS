#pragma once
#include "types.h"

#define PAGE_SIZE 4096

typedef uint8 pflags_t;

enum PAGE_FLAGS
{
	PAGE_PRESENT = 1,
	PAGE_WRITE = 2,
	PAGE_USER = 4
};

struct ADDRESS_SPACE
{
    void* ptr;

	ADDRESS_SPACE()
	{
		ptr = 0;
	}

	bool operator==(const ADDRESS_SPACE& other) const
	{
		return ptr == other.ptr;
	}
};

namespace VMem
{
	void* KernelAlloc(size_t count);
	void* UserAlloc(size_t count);
	void* KernelMapFirstFree(void* phys, size_t count, pflags_t flags);
	void* UserMapFirstFree(void* phys, size_t count, pflags_t flags);
	bool UserAllocShared(ADDRESS_SPACE other_space, void*& addr1, void*& addr2, size_t count);

	ADDRESS_SPACE GetAddressSpace();
    bool SwapAddressSpace(ADDRESS_SPACE& space);
    ADDRESS_SPACE CreateAddressSpace();
	ADDRESS_SPACE CopyAddressSpace();

    size_t GetAddress(size_t virt);
    pflags_t GetFlags(size_t virt);

    void* FirstFree(size_t count, size_t start, size_t end);
    bool MapPages(void* virt, void* phys, size_t count, pflags_t flags);
    bool FreePages(void* virt, size_t count);

    bool Init();
};