#pragma once
#include "types.h"
#include "Memory/memory.h"

#define PAGE_TABLE_LENGTH	1024
#define PAGE_DIR_LENGTH		1024

enum PAGE_TABLE_FLAG
{
	PTE_PRESENT = 1,
	PTE_WRITABLE = 2,
	PTE_USER = 4,
	PTE_WRITETHROUGH = 8,
	PTE_CACHE_DISABLED = 0x10,
	PTE_ACCESSED = 0x20,
	PTE_DIRTY = 0x40,
	PTE_PAT = 0x80,
	PTE_CPU_GLOBAL = 0x100,
	PTE_LV4_GLOBAL = 0x200,
	PTE_FRAME = 0x7FFFF000
};

enum PAGE_DIR_FLAG
{
	PDE_PRESENT = 1,
	PDE_WRITABLE = 2,
	PDE_USER = 4,
	PDE_WRITETHROUGH = 8,
	PDE_CACHE_DISABLED = 0x10,
	PDE_ACCESSED = 0x20,
	PDE_DIRTY = 0x40,
	PDE_4MB = 0x80,
	PDE_CPU_GLOBAL = 0x100,
	PDE_LV4_GLOBAL = 0x200,
	PDE_FRAME = 0x7FFFF000
};

struct PAGE_TABLE
{
	uint32 values[PAGE_TABLE_LENGTH];

	void SetFlags(uint32 index, uint32 flags)
	{
		values[index] = (values[index] & PTE_FRAME) | flags;
	}

	uint32 GetFlags(uint32 index)
	{
		return values[index] & ~PTE_FRAME;
	}

	void SetAddr(uint32 index, void* addr)
	{
		values[index] = (values[index] & ~PTE_FRAME) | (uint32)addr;
	}

	void* GetAddr(uint32 index)
	{
		return (void*)(values[index] & PTE_FRAME);
	}
} __attribute__((packed));

struct PAGE_DIR
{
	uint32 values[PAGE_DIR_LENGTH];

	void SetFlags(uint32 index, uint32 flags)
	{
		values[index] = (values[index] & PDE_FRAME) | flags;
	}

	uint32 GetFlags(uint32 index)
	{
		return values[index] & ~PDE_FRAME;
	}

	void SetTable(uint32 index, PAGE_TABLE* table)
	{
		values[index] = (values[index] & ~PDE_FRAME) | (uint32)table;
	}

	PAGE_TABLE* GetTable(uint32 index)
	{
		return (PAGE_TABLE*)(values[index] & PDE_FRAME);
	}
} __attribute__((packed));

namespace VMem::Arch
{
	size_t GetAddress(size_t virt);
	pflags_t GetFlags(size_t virt);
	
	ADDRESS_SPACE GetAddressSpace();
	bool SwapAddressSpace(ADDRESS_SPACE& space);
	bool CreateAddressSpace(ADDRESS_SPACE& space);
	bool CopyAddressSpace(ADDRESS_SPACE& space);

	void* FirstFree(size_t count, size_t start, size_t end);
	bool MapPages(void* virt, void* phys, size_t count, pflags_t flags);
	bool FreePages(void* virt, size_t count);

	bool Init();
}