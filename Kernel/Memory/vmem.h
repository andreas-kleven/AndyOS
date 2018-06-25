#pragma once
#include "definitions.h"
#include "Boot/multiboot.h"

#define PAGE_SIZE		0x1000

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

	void SetFlag(uint32 index, uint32 flag)
	{
		values[index] |= flag;
	}

	void UnsetFlag(uint32 index, uint32 flag)
	{
		values[index] &= ~flag;
	}

	uint32 GetFlags(uint32 index)
	{
		return values[index] & ~PTE_FRAME;
	}

	void SetAddr(uint32 index, uint32 addr)
	{
		values[index] = (values[index] & ~PTE_FRAME) | addr;
	}

	uint32 GetAddr(uint32 index)
	{
		return values[index] & PTE_FRAME;
	}
} __attribute__((packed));

struct PAGE_DIR
{
	uint32 values[PAGE_DIR_LENGTH];

	void SetFlag(uint32 index, uint32 flag)
	{
		values[index] |= flag;
	}

	void UnsetFlag(uint32 index, uint32 flag)
	{
		values[index] &= ~flag;
	}

	bool GetFlag(uint32 index, uint32 flag)
	{
		return values[index] & flag;
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

class VMem
{
public:
	static void Init(MULTIBOOT_INFO* bootinfo, uint32 kernel_end);
	static bool MapPhysAddr(uint32 phys, uint32 virt, uint32 flags, uint32 blocks);

	static void* KernelAlloc(uint32 blocks);
	static void* UserAlloc(uint32 blocks);
	static void* KernelMapFirstFree(uint32 phys, uint32 flags, uint32 blocks);
	static void* UserMapFirstFree(uint32 phys, uint32 flags, uint32 blocks);
	static void UserAllocShared(PAGE_DIR* dir1, PAGE_DIR* dir2, void*& addr1, void*& addr2, uint32 blocks);

	static uint32 FirstFree(uint32 blocks, uint32 start, uint32 end);

	static void SwitchDir(PAGE_DIR* dir);
	static PAGE_DIR* CreatePageDir();
	static PAGE_DIR* GetCurrentDir();

	static uint32 GetAddress(uint32 virt);
	static uint32 GetFlags(uint32 virt);

private:
	static void* Alloc(uint32 flags, uint32 blocks, uint32 start, uint32 end);
	static void* MapFirstFree(uint32 phys, uint32 flags, uint32 blocks, uint32 start, uint32 end);
	static bool CreatePageTable(uint32 virt, uint32 flags);
	static void EnablePaging();
};