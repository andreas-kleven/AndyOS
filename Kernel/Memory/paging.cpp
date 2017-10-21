#include "paging.h"
#include "pmem.h"
#include "memory.h"
#include "Kernel\kernel.h"
#include "string.h"
#include "debug.h"

#define PAGE_ROOT_DIR_ADDR 0 //1MB

#define PAGE_DIR_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xFFF)

PAGE_DIR* current_dir;

void Paging::Init(MULTIBOOT_INFO* bootinfo)
{
	MULTIBOOT_INFO* info = 0;
	memcpy(info, bootinfo, sizeof(MULTIBOOT_INFO));

	PAGE_DIR* dir = (PAGE_DIR*)Memory::AllocBlocks(1);
	memset(dir, 0, sizeof(PAGE_DIR));

	uint32 flags = PDE_PRESENT | PDE_WRITABLE;

	//Map dir
	MapPhysAddr(dir, (uint32)dir, (uint32)dir, flags);

	//Identity map first 16 MB
	MapPhysAddr(dir, 0, 0, flags, 0x1000);
	MapPhysAddr(dir, 0, 0, flags, 0x100000000 / PAGE_SIZE);

	//Map kernel to higher half
	int blocks = (0x100000000 - KERNEL_BASE) / PAGE_SIZE;
	//MapPhysAddr(dir, KERNEL_BASE_PHYS, KERNEL_BASE, flags, blocks);
	MapPhysAddr(dir, KERNEL_BASE, KERNEL_BASE, flags, blocks);

	//Map framebuffer for debugging
	//MapPhysAddr(dir, 0xE0000000, 0xE0000000, 3, 128 * 1024);

	/*uint32 memptr = PAGE_ROOT_DIR_ADDR + sizeof(PAGE_DIR);

	uint32 addr = 0;

	for (int i = 0; i < 2; i++)
	{
		PAGE_TABLE* table = (PAGE_TABLE*)memptr;
		memptr += PAGE_SIZE;

		for (int j = 0; j < PAGE_TABLE_LENGTH; j++)
		{
			PAGE_TABLE_ENTRY* entry = &table->entries[j];
			entry->SetFlag(PDE_PRESENT | PDE_WRITABLE);
			entry->SetAddr(addr);
			addr += PAGE_SIZE;
		}

		PAGE_DIR_ENTRY* dir_entry = &dir->entries[i];
		dir_entry->SetFlag(PDE_PRESENT | PDE_WRITABLE);
		dir_entry->SetTable(table);
	}

	_asm cli
	_asm hlt*/

	SwitchDir(dir);
	EnablePaging();

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

bool Paging::AllocPage(void* virt)
{
	void* p = Memory::AllocBlocks(1);
	if (!p)
		return 0;

	uint32 addr = (uint32)addr;
	PAGE_DIR_ENTRY* dir_entry = &current_dir->entries[PAGE_DIR_INDEX(addr)];
	PAGE_TABLE_ENTRY* table_entry = &dir_entry->GetTable()->entries[PAGE_TABLE_INDEX(addr)];

	table_entry->SetFlag(PTE_PRESENT);
	table_entry->SetAddr((uint32)addr);
	return 1;
}

//FreePage{}

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
