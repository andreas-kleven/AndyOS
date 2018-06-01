#include "vmem.h"
#include "memory.h"
#include "pmem.h"
#include "Kernel\kernel.h"
#include "string.h"
#include "debug.h"

#define PAGE_ROOT_DIR_ADDR 0 //1MB

#define PAGE_DIR_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xFFF)

PAGE_DIR* current_dir;

void VMem::Init(MULTIBOOT_INFO* bootinfo)
{
	MULTIBOOT_INFO* info = 0;
	memcpy(info, bootinfo, sizeof(MULTIBOOT_INFO));

	//Page dir
	current_dir = (PAGE_DIR*)PMem::AllocBlocks(1);
	memset(current_dir, 0, sizeof(PAGE_DIR));

	//Tables
	PAGE_TABLE* tables = (PAGE_TABLE*)PMem::AllocBlocks(PAGE_DIR_LENGTH);
	memset(tables, 0, sizeof(PAGE_TABLE) * PAGE_DIR_LENGTH);

	for (int i = 0; i < PAGE_DIR_LENGTH; i++)
	{
		PAGE_TABLE* table = tables + i;
		current_dir->entries[i].SetTable(table);
	}

	uint32 flags = PDE_PRESENT | PDE_WRITABLE;

	//Map page dir and tables
	MapPhysAddr(current_dir, (uint32)current_dir, (uint32)current_dir, flags);
	MapPhysAddr(current_dir, (uint32)tables, (uint32)tables, flags, PAGE_DIR_LENGTH);

	//Identity map first 1 MB
	MapPhysAddr(current_dir, 0, 0, flags, 0x100000 / BLOCK_SIZE);

	//Map kernel and memory map
	MapPhysAddr(current_dir, KERNEL_BASE_PHYS, KERNEL_BASE, flags, (KERNEL_SIZE + MEMORY_MAP_SIZE) / BLOCK_SIZE);

	SwitchDir(current_dir);
	EnablePaging();

	Kernel::HigherHalf(info);
}

bool VMem::MapPhysAddr(PAGE_DIR* dir, uint32 phys, uint32 virt, uint32 flags)
{
	PAGE_DIR_ENTRY* dir_entry = &dir->entries[PAGE_DIR_INDEX(virt)];

	if (dir_entry->value == 0)
		CreatePageTable(dir, virt, flags);

	dir_entry->SetFlag(flags);

	PAGE_TABLE* table = dir_entry->GetTable();
	PAGE_TABLE_ENTRY* table_entry = &table->entries[PAGE_TABLE_INDEX(virt)];
	table_entry->SetFlag(flags);
	table_entry->SetAddr(phys);
	return 1;
}

bool VMem::MapPhysAddr(PAGE_DIR* dir, uint32 phys, uint32 virt, uint32 flags, uint32 blocks)
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

PAGE_DIR* VMem::GetCurrentDir()
{
	return current_dir;
}

bool VMem::CreatePageTable(PAGE_DIR* dir, uint32 virt, uint32 flags)
{
	PAGE_DIR_ENTRY* dir_entry = &dir->entries[PAGE_DIR_INDEX(virt)];

	if (dir_entry->value)
		return 0;

	PAGE_TABLE* table = (PAGE_TABLE*)PMem::AllocBlocks(1);

	if (!table)
		return 0;

	memset(table, 0, PAGE_SIZE);
	dir_entry->SetFlag(flags);
	dir_entry->SetTable(table);
	MapPhysAddr(dir, (uint32)table, (uint32)table, flags);
	return 1;
}

void VMem::SwitchDir(PAGE_DIR* dir)
{
	uint32 addr = (uint32)&dir->entries;
	current_dir = dir;

	_asm
	{
		mov	eax, [addr]
		mov	cr3, eax
	}
}

void VMem::EnablePaging()
{
	_asm
	{
		mov	eax, cr0
		or eax, 0x80000000
		mov	cr0, eax
	}
}
