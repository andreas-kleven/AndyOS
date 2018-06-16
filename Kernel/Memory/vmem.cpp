#include "vmem.h"
#include "memory.h"
#include "Kernel/kernel.h"
#include "string.h"
#include "Lib/debug.h"
#include "../Drawing/vbe.h"

#define PAGE_DIR_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xFFF)

PAGE_DIR* current_dir = 0;
PAGE_DIR* main_dir;

void VMem::Init(MULTIBOOT_INFO* bootinfo, uint32 kernel_end)
{
	MULTIBOOT_INFO info = *bootinfo;
	VBE_MODE_INFO vbe_info = *(VBE_MODE_INFO*)bootinfo->vbe_mode_info;
	info.vbe_mode_info = (uint32)&vbe_info;

	//Page dir
	main_dir = (PAGE_DIR*)PMem::AllocBlocks(1);
	memset(main_dir, 0, sizeof(PAGE_DIR));

	//Tables
	PAGE_TABLE* tables = (PAGE_TABLE*)PMem::AllocBlocks(PAGE_DIR_LENGTH);
	memset(tables, 0, sizeof(PAGE_TABLE) * PAGE_DIR_LENGTH);

	for (int i = 0; i < PAGE_DIR_LENGTH; i++)
	{
		PAGE_TABLE* table = tables + i;
		main_dir->entries[i].SetFlag(PDE_PRESENT | PDE_WRITABLE);
		main_dir->entries[i].SetTable(table);
	}

	uint32 flags = PDE_PRESENT | PDE_WRITABLE;

	//Map page dir and tables
	MapPhysAddr(main_dir, (uint32)main_dir, (uint32)main_dir, flags, 1);
	MapPhysAddr(main_dir, (uint32)tables, (uint32)tables, flags, PAGE_DIR_LENGTH);

	//Map kernel and memory map
	MapPhysAddr(main_dir, KERNEL_BASE, KERNEL_BASE, flags, BYTES_TO_BLOCKS(kernel_end - KERNEL_BASE + MEMORY_MAP_SIZE));

	SwitchDir(main_dir);
	EnablePaging();

	Kernel::HigherHalf(info);
}

bool VMem::MapPhysAddr(PAGE_DIR* dir, uint32 phys, uint32 virt, uint32 flags, uint32 blocks)
{
	for (int i = 0; i < blocks; i++)
	{
		PAGE_DIR_ENTRY* dir_entry = &dir->entries[PAGE_DIR_INDEX(virt)];

		if (dir_entry->value == 0)
			CreatePageTable(dir, virt, flags);

		dir_entry->SetFlag(flags);

		PAGE_TABLE* table = dir_entry->GetTable();
		PAGE_TABLE_ENTRY* table_entry = &table->entries[PAGE_TABLE_INDEX(virt)];
		table_entry->SetFlag(flags);
		table_entry->SetAddr(phys);

		phys += PAGE_SIZE;
		virt += PAGE_SIZE;

		asm volatile("invlpg (%0)" :: "r" (virt));
	}

	return 1;
}

void* VMem::KernelAlloc(uint32 blocks)
{
	return Alloc(main_dir, PTE_PRESENT | PTE_WRITABLE | PTE_USER, blocks, KERNEL_BASE, KERNEL_END);
}

void* VMem::UserAlloc(PAGE_DIR* dir, uint32 blocks)
{
	return Alloc(dir, PTE_PRESENT | PTE_WRITABLE | PTE_USER, blocks, USER_BASE, USER_END);
}

void* VMem::KernelMapFirstFree(uint32 phys, uint32 flags, uint32 blocks)
{
	return MapFirstFree(main_dir, phys, flags, blocks, KERNEL_BASE, KERNEL_END);
}

void* VMem::UserMapFirstFree(PAGE_DIR* dir, uint32 phys, uint32 flags, uint32 blocks)
{
	return MapFirstFree(dir, phys, flags, blocks, USER_BASE, USER_END);
}

uint32 VMem::FirstFree(PAGE_DIR* dir, uint32 blocks, uint32 start, uint32 end)
{
	for (int i = start; i < end; i += PAGE_SIZE)
	{
		if (!GetTableEntry(dir, i)->GetFlag(PTE_PRESENT))
		{
			bool found = true;

			for (int j = 1; j < blocks; j++)
			{
				if (GetTableEntry(dir, i + j * PAGE_SIZE)->GetFlag(PTE_PRESENT))
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
	//Dir
	PAGE_DIR* dir = (PAGE_DIR*)KernelAlloc(1);
	memset(dir, 0, sizeof(PAGE_DIR));

	//Tables
	PAGE_TABLE* tables = (PAGE_TABLE*)KernelAlloc(768);
	memset(tables, 0, sizeof(PAGE_TABLE) * 768);

	for (int i = 0; i < 256; i++)
	{
		dir->entries[i] = main_dir->entries[i];
	}

	for (int i = 256; i < PAGE_DIR_LENGTH; i++)
	{
		PAGE_TABLE* table = tables + i - 256;
		dir->entries[i].SetTable(table);
	}

	return dir;
}

PAGE_DIR* VMem::GetCurrentDir()
{
	return current_dir;
}

PAGE_TABLE_ENTRY* VMem::GetTableEntry(PAGE_DIR* dir, uint32 virt)
{
	PAGE_DIR_ENTRY* dir_entry = &dir->entries[PAGE_DIR_INDEX(virt)];
	PAGE_TABLE* table = dir_entry->GetTable();
	return &table->entries[PAGE_TABLE_INDEX(virt)];
}

//Private
void* VMem::Alloc(PAGE_DIR* dir, uint32 flags, uint32 blocks, uint32 start, uint32 end)
{
	uint32 virt = FirstFree(dir, blocks, start, end);
	uint32 phys = (uint32)PMem::AllocBlocks(blocks);
	MapPhysAddr(dir, phys, virt, flags, blocks);
	return (void*)virt;

	uint32 _virt = virt;

	if (!virt)
		return 0;

	for (int i = 0; i < blocks; i++)
	{
		uint32 phys = (uint32)PMem::AllocBlocks(1);

		if (!phys)
			return 0;

		if (!MapPhysAddr(dir, phys, _virt, flags, 1))
			return 0;

		_virt += PAGE_SIZE;
	}

	return (void*)virt;
}

void* VMem::MapFirstFree(PAGE_DIR* dir, uint32 phys, uint32 flags, uint32 blocks, uint32 start, uint32 end)
{
	uint32 virt = FirstFree(dir, blocks, start, end);

	if (!virt)
		return 0;

	if (MapPhysAddr(dir, phys, (uint32)virt, flags, blocks))
		return (void*)virt;

	return 0;
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
	MapPhysAddr(dir, (uint32)table, (uint32)table, flags, 1);
	return 1;
}

void VMem::EnablePaging()
{
	asm volatile(
		"mov %cr0, %eax\n"
		"or $0x80000000, %eax\n"
		"mov %eax, %cr0");
}
