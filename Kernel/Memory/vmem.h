#pragma once
#include "definitions.h"
#include "Boot\multiboot.h"

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

struct PAGE_TABLE;

struct PAGE_TABLE_ENTRY
{
	uint32 value = 0;

	void SetFlag(uint32 flag)
	{
		value |= flag;
	}

	void UnsetFlag(uint32 flag)
	{
		value &= ~flag;
	}

	bool GetFlag(uint32 flag)
	{
		return value & flag;
	}

	void SetAddr(uint32 addr)
	{
		value = (value & ~PTE_FRAME) | addr;
	}
};

struct PAGE_DIR_ENTRY
{
	uint32 value = 0;

	void SetFlag(uint32 flag)
	{
		value |= flag;
	}

	void UnsetFlag(uint32 flag)
	{
		value &= ~flag;
	}

	bool GetFlag(uint32 flag)
	{
		return value & flag;
	}

	void SetTable(PAGE_TABLE* table)
	{
		value = (value & ~PDE_FRAME) | (uint32)table;
	}

	PAGE_TABLE* GetTable()
	{
		return (PAGE_TABLE*)(value & PDE_FRAME);
	}
};

struct PAGE_TABLE
{
	PAGE_TABLE_ENTRY entries[PAGE_TABLE_LENGTH];
};

struct PAGE_DIR
{
	PAGE_DIR_ENTRY entries[PAGE_DIR_LENGTH];
};

static class VMem
{
public:
	static void Init(MULTIBOOT_INFO* bootinfo);
	static bool MapPhysAddr(PAGE_DIR* dir, uint32 phys, uint32 virt, uint32 flags, uint32 blocks);

	static void* KernelAlloc(uint32 blocks);
	static void* UserAlloc(PAGE_DIR* dir, uint32 blocks);
	static void* KernelMapFirstFree(uint32 phys, uint32 flags, uint32 blocks);
	static void* UserMapFirstFree(PAGE_DIR* dir, uint32 phys, uint32 flags, uint32 blocks);

	static uint32 FirstFree(PAGE_DIR* dir, uint32 blocks, uint32 start, uint32 end);

	static void SwitchDir(PAGE_DIR* dir);
	static PAGE_DIR* CreatePageDir();
	static PAGE_DIR* GetCurrentDir();
	static PAGE_TABLE_ENTRY* GetTableEntry(PAGE_DIR* dir, uint32 virt);

private:
	static void* Alloc(PAGE_DIR* dir, uint32 flags, uint32 blocks, uint32 start, uint32 end);
	static void* MapFirstFree(PAGE_DIR* dir, uint32 phys, uint32 flags, uint32 blocks, uint32 start, uint32 end);
	static bool CreatePageTable(PAGE_DIR* dir, uint32 virt, uint32 flags);
	static void EnablePaging();
};