#include "paging.h"
#include "pmem.h"
#include "memory.h"
#include "string.h"
#include "debug.h"

#define PAGE_DIR_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xFFF)

STATUS Paging::Init()
{
	PAGE_DIR* dir = (PAGE_DIR*)Memory::AllocBlocks(1);
	memset(dir, 0, sizeof(PAGE_DIR));

	PAGE_TABLE* table = (PAGE_TABLE*)Memory::AllocBlocks(1);

	uint32 addr = 0;
	for (int i = 0; i < PAGES_PER_TABLE; i++, addr += PAGE_SIZE)
	{
		PAGE_TABLE_ENTRY entry;
		entry.SetFlag(PTE_PRESENT);
		entry.SetFlag(PTE_USER);
		entry.SetAddr(addr);
		table->entries[i] = entry;
	}

	PAGE_DIR_ENTRY* dir_entry = &dir->entries[0];
	dir_entry->SetFlag(PDE_PRESENT);
	dir_entry->SetFlag(PDE_WRITABLE);
	dir_entry->SetFlag(PDE_USER);
	dir_entry->SetTable(table);

	//PAGE_DIR_ENTRY* dir_entry = &dir.entries[0];
	//dir_entry->SetFlag(PDE_PRESENT);
	//dir_entry->SetFlag(PDE_WRITABLE);
	//dir_entry->SetFlag(PDE_USER);
	//dir_entry->SetAddr((uint32)table);

	/*uint32 addr = 0;
	for (int i = 0; i < 1; i++)
	{
		PAGE_TABLE* table = (PAGE_TABLE*)Memory::AllocBlocks(1);

		for (int j = 0; j < PAGES_PER_TABLE; j++)
		{
			PAGE_TABLE_ENTRY entry;
			entry.SetFlag(PTE_PRESENT);
			entry.SetFlag(PTE_USER);
			entry.SetAddr(addr);
			table->entries[j] = entry;

			addr += PAGE_SIZE;
		}

		PAGE_DIR_ENTRY* dir_entry = &dir->entries[i];
		dir_entry->SetFlag(PDE_PRESENT);
		dir_entry->SetFlag(PDE_WRITABLE);
		dir_entry->SetFlag(PDE_USER);
		dir_entry->SetTable(table);
	}*/

	addr = KERNEL_BASE_PHYS;
	for (int i = 768; i < PAGES_PER_DIR; i++)
	{
		PAGE_TABLE* table = (PAGE_TABLE*)Memory::AllocBlocks(1);

		for (int j = 0; j < PAGES_PER_TABLE; j++)
		{
			PAGE_TABLE_ENTRY entry;
			entry.SetFlag(PTE_PRESENT);
			entry.SetFlag(PTE_USER);
			entry.SetAddr(addr);
			table->entries[j] = entry;

			addr += PAGE_SIZE;
		} 

		PAGE_DIR_ENTRY* dir_entry = &dir->entries[i];
		dir_entry->SetFlag(PDE_PRESENT);
		dir_entry->SetFlag(PDE_WRITABLE);
		dir_entry->SetFlag(PDE_USER);
		dir_entry->SetTable(table);
	}

	//Map framebuffer for debugging
	addr = 0xE0000000;
	for (int i = 896; i < 1024; i++)
	{
		PAGE_TABLE* table = (PAGE_TABLE*)Memory::AllocBlocks(1);

		for (int j = 0; j < PAGES_PER_TABLE; j++)
		{
			PAGE_TABLE_ENTRY entry;
			entry.SetFlag(PTE_PRESENT);
			entry.SetFlag(PTE_USER);
			entry.SetAddr(addr);
			table->entries[j] = entry;

			addr += PAGE_SIZE;
		}

		PAGE_DIR_ENTRY* dir_entry = &dir->entries[i];
		dir_entry->SetFlag(PDE_PRESENT);
		dir_entry->SetFlag(PDE_WRITABLE);
		dir_entry->SetFlag(PDE_USER);
		dir_entry->SetTable(table);
	}

	LoadDir((uint32)&dir->entries);

	EnablePaging();

	*(uint32*)(0xE0000000) = 0xFFFF00;
	*(uint32*)(0xE0000004) = 0xFFFF00;
	*(uint32*)(0xE0000008) = 0xFFFF00;
	*(uint32*)(0xE000000C) = 0xFFFF00;
	*(uint32*)(0xE0000010) = 0xFFFF00;
	_asm cli
	_asm hlt

	//Debug::Print("Paging enabled\n");

	//_asm cli
	//_asm hlt

	return STATUS_SUCCESS;
}

void Paging::LoadDir(uint32 addr)
{
	_asm
	{
		mov	eax, [addr]
		mov	cr3, eax
	}
}

#include "Kernel\kernel.h"
__declspec (naked) void hello()
{
	_asm lea eax, Kernel::Main;
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
