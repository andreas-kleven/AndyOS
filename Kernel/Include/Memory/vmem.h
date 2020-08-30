#pragma once
#include <types.h>

#define PAGE_SIZE 4096

typedef uint8 pflags_t;

enum PAGE_FLAGS
{
	PAGE_NONE = 0,
	PAGE_PRESENT = 1,
	PAGE_WRITE = 2,
	PAGE_USER = 4
};

struct ADDRESS_SPACE
{
	void *ptr;

	ADDRESS_SPACE()
	{
		ptr = 0;
	}

	bool operator==(const ADDRESS_SPACE &other) const
	{
		return ptr == other.ptr;
	}
};

struct PAGE_INFO
{
	uint32 refs;
	uint32 cow;
	bool writable;
} __attribute__((packed));

namespace VMem
{
	extern PAGE_INFO *page_list;

	PAGE_INFO *GetInfo(size_t virt);
	size_t GetAddress(size_t virt);
	pflags_t GetFlags(size_t virt);

	ADDRESS_SPACE GetAddressSpace();
	bool SwapAddressSpace(ADDRESS_SPACE &space);
	bool CreateAddressSpace(ADDRESS_SPACE &space);
	bool DestroyAddressSpace(ADDRESS_SPACE &space);
	bool CopyAddressSpace(ADDRESS_SPACE &space);
	bool CopyOnWrite(void *virt1, void *virt2, size_t count, ADDRESS_SPACE &to);
	bool PerformCopy(void *virt);

	void *FirstFree(size_t count, size_t start, size_t end);
	bool MapPages(void *virt, void *phys, size_t count, pflags_t flags);
	bool FreePages(void *virt, size_t count);

	void *KernelAlloc(size_t count, bool contiguous = false);
	void *UserAlloc(size_t count, bool contiguous = false);
	void *KernelMapFirstFree(void *phys, size_t count, pflags_t flags);
	void *UserMapFirstFree(void *phys, size_t count, pflags_t flags);
	bool UserAllocShared(ADDRESS_SPACE other_space, void *&addr1, void *&addr2, size_t count);

	bool Init();
}; // namespace VMem
