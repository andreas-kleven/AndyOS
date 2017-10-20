#include "paging.h"
#include "pmem.h"
#include "memory.h"
#include "Kernel\kernel.h"
#include "string.h"
#include "debug.h"

#define PAGE_DIR_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xFFF)

PAGE_DIR* current_dir;

void Paging::Init(MULTIBOOT_INFO* bootinfo)
{
	MULTIBOOT_INFO* info = 0;
	memcpy(info, bootinfo, sizeof(MULTIBOOT_INFO));

	//PAGE_DIR* dir = (PAGE_DIR*)Memory::AllocBlocks(1);
	PAGE_DIR* dir = (PAGE_DIR*)0x1000;
	memset(dir, 0, sizeof(PAGE_DIR));

	uint32 flags = PDE_PRESENT | PDE_WRITABLE;

	//Identity map first 16 MB
	MapPhysAddr(dir, 0, 0, flags, 0x1000);

	//Map kernel to higher half
	int blocks = (0x100000000 - KERNEL_BASE) / PAGE_SIZE;
	MapPhysAddr(dir, KERNEL_BASE_PHYS, KERNEL_BASE, flags, blocks);

	//Map framebuffer for debugging
	//MapPhysAddr(dir, 0xE0000000, 0xE0000000, 3, 128 * 1024);

	SwitchDir(dir);
	EnablePaging();
	current_dir = dir;

	Kernel::HigherHalf(info);
}

bool Paging::MapPhysAddr(PAGE_DIR* dir, uint32 phys, uint32 virt, uint32 flags)
{
	PAGE_DIR_ENTRY* dir_entry = &dir->entries[PAGE_DIR_INDEX(virt)];

	if (dir_entry->value == 0)
		CreatePageTable(dir, virt, flags);

	PAGE_TABLE* table = dir_entry->GetTable();
	PAGE_TABLE_ENTRY* table_entry = &table->entries[PAGE_TABLE_INDEX(virt)];
	table_entry->SetFlag(flags);
	table_entry->SetAddr(phys);
	return 1;
}

bool Paging::MapPhysAddr(PAGE_DIR* dir, uint32 phys, uint32 virt, uint32 flags, uint32 blocks)
{
	for (int i = 0; i < blocks; i++)
	{
		if (!MapPhysAddr(dir, phys, virt, flags))
			return 0;

		phys += PAGE_SIZE;
		virt += PAGE_SIZE;
	}

	return 1;
}

PAGE_DIR* Paging::GetCurrentDir()
{
	return current_dir;
}

bool Paging::CreatePageTable(PAGE_DIR* dir, uint32 virt, uint32 flags)
{
	PAGE_DIR_ENTRY* dir_entry = &dir->entries[PAGE_DIR_INDEX(virt)];

	if (dir_entry->value)
		return 0;

	PAGE_TABLE* table = (PAGE_TABLE*)Memory::AllocBlocks(1);

	if (!table)
		return 0;

	memset(table, 0, PAGE_SIZE);
	dir_entry->SetFlag(flags);
	dir_entry->SetTable(table);
	MapPhysAddr(dir, (uint32)table, (uint32)table, flags);
	return 1;
}

void Paging::SwitchDir(PAGE_DIR* dir)
{
	uint32 addr = (uint32)&dir->entries;
	current_dir = dir;

	_asm
	{
		mov	eax, [addr]
		mov	cr3, eax
	}
}

void Paging::EnablePaging()
{
	_asm
	{
		mov	eax, cr0
		or eax, 0x80000000
		mov	cr0, eax
	}
}
