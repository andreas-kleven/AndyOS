#include "vmem.h"
#include "memory.h"
#include "Kernel/kernel.h"
#include "string.h"
#include "Lib/debug.h"
#include "../Drawing/vbe.h"

#define PAGE_DIR_INDEX(x) 	(((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)

#define PAGE_DIR_ADDRESS 0xFFFFF000
#define PAGE_TABLE_ADDRESS(index) (0xFFC00000 + ((index)) * PAGE_SIZE)

#define PAGE_TABLE_AT(virt) (PAGE_TABLE*)(PAGE_TABLE_ADDRESS(PAGE_DIR_INDEX((virt))));

PAGE_DIR* current_dir;
PAGE_DIR* main_dir;

PAGE_DIR* GetCurrentDirVirt()
{
	if (!current_dir)
		return main_dir;

	return (PAGE_DIR*)PAGE_DIR_ADDRESS;
}

void VMem::Init(MULTIBOOT_INFO* bootinfo, uint32 kernel_end)
{
	current_dir = 0;

	MULTIBOOT_INFO info = *bootinfo;
	VBE_MODE_INFO vbe_info = *(VBE_MODE_INFO*)bootinfo->vbe_mode_info;
	info.vbe_mode_info = (uint32)&vbe_info;

	//Page dir
	main_dir = (PAGE_DIR*)PMem::AllocBlocks(1);
	memset(main_dir, 0, sizeof(PAGE_DIR));

	//Tables
	PAGE_TABLE* tables = (PAGE_TABLE*)PMem::AllocBlocks(PAGE_DIR_LENGTH);
	memset(tables, 0, sizeof(PAGE_TABLE) * PAGE_DIR_LENGTH);

	uint32 flags = PDE_PRESENT | PDE_WRITABLE;

	for (int i = 0; i < PAGE_DIR_LENGTH; i++)
	{
		PAGE_TABLE* table = tables + i;
		main_dir->SetFlag(i, flags);
		main_dir->SetTable(i, table);
	}

	//Map page dir
	MapPhysAddr((uint32)main_dir, (uint32)main_dir, flags, 1);
	MapPhysAddr((uint32)main_dir, PAGE_DIR_ADDRESS, flags, 1);

	//Map tables
	MapPhysAddr((uint32)tables, (uint32)tables, flags, PAGE_DIR_LENGTH);
	MapPhysAddr((uint32)tables, PAGE_TABLE_ADDRESS(0), flags, PAGE_DIR_LENGTH - 1);

	//Map kernel and memory map
	MapPhysAddr(KERNEL_BASE, KERNEL_BASE, flags, BYTES_TO_BLOCKS(kernel_end - KERNEL_BASE + MEMORY_MAP_SIZE));

	SwitchDir(main_dir);
	EnablePaging();

	Kernel::HigherHalf(info);
}

bool VMem::MapPhysAddr(uint32 phys, uint32 virt, uint32 flags, uint32 blocks)
{
	PAGE_DIR* dir = GetCurrentDirVirt();

	for (int i = 0; i < blocks; i++)
	{
		int index = PAGE_DIR_INDEX(virt);
		int table_index = PAGE_TABLE_INDEX(virt);

		if (dir->GetTable(index) == 0)
			CreatePageTable(virt, flags);

		dir->SetFlag(index, flags);

		PAGE_TABLE* table;

		if (current_dir)
		{
			table = PAGE_TABLE_AT(virt);
		}
		else
		{
			table = dir->GetTable(index);
		}

		table->SetFlag(table_index, flags);
		table->SetAddr(table_index, phys);

		phys += PAGE_SIZE;
		virt += PAGE_SIZE;

		asm volatile("invlpg (%0)" :: "r" (virt));
	}

	return 1;
}

void* VMem::KernelAlloc(uint32 blocks)
{
	return Alloc(PTE_PRESENT | PTE_WRITABLE | PTE_USER, blocks, KERNEL_BASE, KERNEL_END);
}

void* VMem::UserAlloc(uint32 blocks)
{
	return Alloc(PTE_PRESENT | PTE_WRITABLE | PTE_USER, blocks, USER_BASE, USER_END);
}

void* VMem::KernelMapFirstFree(uint32 phys, uint32 flags, uint32 blocks)
{
	return MapFirstFree(phys, flags, blocks, KERNEL_BASE, KERNEL_END);
}

void* VMem::UserMapFirstFree(uint32 phys, uint32 flags, uint32 blocks)
{
	return MapFirstFree(phys, flags, blocks, USER_BASE, USER_END);
}

uint32 VMem::FirstFree(uint32 blocks, uint32 start, uint32 end)
{
	for (int i = start; i < end; i += PAGE_SIZE)
	{
		if (!GetFlags(i) & PTE_PRESENT)
		{
			bool found = true;

			for (int j = 1; j < blocks; j++)
			{
				if (GetFlags(i + j * PAGE_SIZE) & PTE_PRESENT)
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				return i;
			}
		}
	}

	return 0;
}

void VMem::SwitchDir(PAGE_DIR* dir)
{
	if (dir == current_dir)
		return;

	current_dir = dir;

	asm volatile(
		"mov (%0), %%eax\n"
		"mov %%eax, %%cr3" 
		:: "r" (&dir));
}

PAGE_DIR* VMem::CreatePageDir()
{
	uint32 flags = PDE_PRESENT | PDE_WRITABLE;

	//Dir
	PAGE_DIR* phys = (PAGE_DIR*)PMem::AllocBlocks(1);
	PAGE_DIR* dir = (PAGE_DIR*)KernelMapFirstFree((uint32)phys, flags, 1);
	memset(dir, 0, sizeof(PAGE_DIR));

	//Tables
	PAGE_TABLE* tables_phys = (PAGE_TABLE*)PMem::AllocBlocks(768);
	PAGE_TABLE* tables_virt = (PAGE_TABLE*)(KernelMapFirstFree((uint32)tables_phys, flags, 768));
	memset(tables_virt, 0, sizeof(PAGE_TABLE) * 768);

	tables_phys -= 256;
	tables_virt -= 256;

	for (int i = 0; i < 256; i++)
	{
		dir->values[i] = main_dir->values[i];
	}

	for (int i = 256; i < PAGE_DIR_LENGTH; i++)
	{
		dir->SetTable(i, tables_phys + i);
	}

	//Map tables and directory to last table
	PAGE_TABLE* cur_last_table = main_dir->GetTable(PAGE_DIR_LENGTH - 1);
	PAGE_TABLE* last_table = &tables_virt[PAGE_DIR_LENGTH - 1];
	dir->SetFlag(PAGE_DIR_LENGTH - 1, flags);

	for (int i = 0; i < 256; i++)
	{
		last_table->values[i] = cur_last_table->values[i];	
	}

	for (int i = 256; i < 1023; i++)
	{
		last_table->SetFlag(i, flags);
		last_table->SetAddr(i, (uint32)(tables_phys + i));
	}

	last_table->SetFlag(PAGE_TABLE_LENGTH - 1, flags);
	last_table->SetAddr(PAGE_TABLE_LENGTH - 1, (uint32)phys);

	return phys;
}

PAGE_DIR* VMem::GetCurrentDir()
{
	return current_dir;
}

uint32 VMem::GetAddress(uint32 virt)
{
	PAGE_TABLE* table = PAGE_TABLE_AT(virt);
	return table->GetAddr(PAGE_TABLE_INDEX(virt));
}

uint32 VMem::GetFlags(uint32 virt)
{
	PAGE_TABLE* table = PAGE_TABLE_AT(virt);
	return table->GetFlags(PAGE_TABLE_INDEX(virt));
}

//Private
void* VMem::Alloc(uint32 flags, uint32 blocks, uint32 start, uint32 end)
{
	uint32 virt = FirstFree(blocks, start, end);
	uint32 phys = (uint32)PMem::AllocBlocks(blocks);
	MapPhysAddr(phys, virt, flags, blocks);
	return (void*)virt;

	uint32 _virt = virt;

	if (!virt)
		return 0;

	for (int i = 0; i < blocks; i++)
	{
		uint32 phys = (uint32)PMem::AllocBlocks(1);

		if (!phys)
			return 0;

		if (!MapPhysAddr(phys, _virt, flags, 1))
			return 0;

		_virt += PAGE_SIZE;
	}

	return (void*)virt;
}

void* VMem::MapFirstFree(uint32 phys, uint32 flags, uint32 blocks, uint32 start, uint32 end)
{
	uint32 virt = FirstFree(blocks, start, end);

	if (!virt)
		return 0;

	if (MapPhysAddr(phys, (uint32)virt, flags, blocks))
	{
		PMem::DeInitRegion((void*)phys, blocks * BLOCK_SIZE);
		return (void*)virt;
	}

	return 0;
}

bool VMem::CreatePageTable(uint32 virt, uint32 flags)
{
	return 0;
	/*PAGE_DIR* dir = (PAGE_DIR*)PAGE_DIR_ADDRESS;
	PAGE_DIR_ENTRY* dir_entry = &dir->entries[PAGE_DIR_INDEX(virt)];

	if (dir_entry->value)
		return 0;

	PAGE_TABLE* table = (PAGE_TABLE*)PMem::AllocBlocks(1);

	if (!table)
		return 0;

	memset(table, 0, PAGE_SIZE);
	dir_entry->SetFlag(flags);
	dir_entry->SetTable(table);
	MapPhysAddr((uint32)table, (uint32)table, flags, 1);
	return 1;*/
}

void VMem::EnablePaging()
{
	asm volatile(
		"mov %cr0, %eax\n"
		"or $0x80000000, %eax\n"
		"mov %eax, %cr0");
}
