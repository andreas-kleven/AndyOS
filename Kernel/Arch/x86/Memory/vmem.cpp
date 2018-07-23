#include "Arch/memory.h"
#include "string.h"

#define PAGE_DIR_INDEX(x) 	((size_t(x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) ((size_t(x) >> 12) & 0x3FF)

#define PAGE_DIR_ADDRESS 0xFFFFF000
#define PAGE_TABLE_ADDRESS(index) (0xFFC00000 + ((index)) * PAGE_SIZE)

#define PAGE_TABLE_AT(virt) (PAGE_TABLE*)(PAGE_TABLE_ADDRESS(PAGE_DIR_INDEX((virt))));

#define F_PF(flags) (pflags_t)((PAGE_PRESENT * (bool)((flags) & PTE_PRESENT)) | (PAGE_WRITE * (bool)((flags) & PTE_WRITABLE)) | (PAGE_USER * (bool)((flags) & PTE_USER)))
#define PF_F(pflags) ((PTE_PRESENT * (bool)((pflags) & PAGE_PRESENT)) | (PTE_WRITABLE * (bool)((pflags) & PAGE_WRITE)) | (PTE_USER * (bool)((pflags) & PAGE_USER)))

extern size_t __KERNEL_START, __KERNEL_END;

namespace VMem::Arch
{
    PAGE_DIR* main_dir;
    ADDRESS_SPACE current_space;

    void EnablePaging()
    {
        asm volatile(
            "mov %cr0, %eax\n"
            "or $0x80000000, %eax\n"
            "mov %eax, %cr0");
    }

	PAGE_DIR* GetCurrentDirVirt()
	{
		if (!current_space.ptr)
			return main_dir;

		return (PAGE_DIR*)PAGE_DIR_ADDRESS;
	}

    ADDRESS_SPACE GetAddressSpace()
    {
        return current_space;
    }

    bool SwapAddressSpace(ADDRESS_SPACE& addr_space)
    {
        if (addr_space == current_space)
			return true;

		current_space = addr_space;

		asm volatile(
			"mov (%0), %%eax\n"
			"mov %%eax, %%cr3" 
			:: "r" (&addr_space.ptr));

		return true;
    }

    ADDRESS_SPACE CreateAddressSpace()
    {
        pflags_t flags = PAGE_PRESENT | PAGE_WRITE;
		uint32 pt_flags = PF_F(flags);

		//Dir
		PAGE_DIR* phys = (PAGE_DIR*)PMem::AllocBlocks(1);
		PAGE_DIR* dir = (PAGE_DIR*)KernelMapFirstFree(phys, 1, flags);
		memset(dir, 0, sizeof(PAGE_DIR));

		//Tables
		PAGE_TABLE* tables_phys = (PAGE_TABLE*)PMem::AllocBlocks(768);
		PAGE_TABLE* tables_virt = (PAGE_TABLE*)(KernelMapFirstFree(tables_phys, 768, flags));
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
		dir->SetFlag(PAGE_DIR_LENGTH - 1, pt_flags);

		for (int i = 0; i < 256; i++)
		{
			last_table->values[i] = cur_last_table->values[i];	
		}

		for (int i = 256; i < 1023; i++)
		{
			last_table->SetFlag(i, pt_flags);
			last_table->SetAddr(i, tables_phys + i);
		}

		last_table->SetFlag(PAGE_TABLE_LENGTH - 1, pt_flags);
		last_table->SetAddr(PAGE_TABLE_LENGTH - 1, phys);

        ADDRESS_SPACE as;
        as.ptr = phys;
		return as;
    }

    size_t GetAddress(size_t virt)
    {
		PAGE_TABLE* table = PAGE_TABLE_AT(virt);
		return (size_t)table->GetAddr(PAGE_TABLE_INDEX(virt));
	}

	uint32 GetPTFlags(size_t virt)
	{
		PAGE_TABLE* table = PAGE_TABLE_AT(virt);
		return table->GetFlags(PAGE_TABLE_INDEX(virt));
	}

    pflags_t GetFlags(size_t virt)
	{        
		return F_PF(GetPTFlags(virt));
    }

    void* FirstFree(size_t count, size_t start, size_t end)
    {
        for (int i = start; i < end; i += PAGE_SIZE)
		{
			if (!GetPTFlags(i) & PTE_PRESENT)
			{
				bool found = true;

				for (int j = 1; j < count; j++)
				{
					if (GetPTFlags(i + j * PAGE_SIZE) & PTE_PRESENT)
					{
						found = false;
						break;
					}
				}

				if (found)
				{
					return (void*)i;
				}
			}
		}
    }

    bool MapPages(void* virt, void* phys, size_t count, pflags_t flags)
    {
        PAGE_DIR* dir = GetCurrentDirVirt();
		uint32 pt_flags = PF_F(flags);

		for (int i = 0; i < count; i++)
		{
			int index = PAGE_DIR_INDEX(virt);
			int table_index = PAGE_TABLE_INDEX(virt);

			//if (dir->GetTable(index) == 0)
			//	CreatePageTable(virt, pt_flags);

			dir->SetFlag(index, pt_flags);

			PAGE_TABLE* table;

			if (current_space.ptr)
			{
				table = PAGE_TABLE_AT(virt);
			}
			else
			{
				table = dir->GetTable(index);
			}

			table->SetFlag(table_index, pt_flags);
			table->SetAddr(table_index, phys);

			phys += PAGE_SIZE;
			virt += PAGE_SIZE;

			asm volatile("invlpg (%0)" :: "r" (virt));
		}

		return 1;
    }

    bool FreePages(void* virt, size_t count)
    {

    }

    bool Init()
    {
        current_space.ptr = 0;

		//Page dir
		main_dir = (PAGE_DIR*)PMem::AllocBlocks(1);
		memset(main_dir, 0, sizeof(PAGE_DIR));

		//Tables
		PAGE_TABLE* tables = (PAGE_TABLE*)PMem::AllocBlocks(PAGE_DIR_LENGTH);
		memset(tables, 0, sizeof(PAGE_TABLE) * PAGE_DIR_LENGTH);

		pflags_t flags = PAGE_PRESENT | PAGE_WRITE;

		for (int i = 0; i < PAGE_DIR_LENGTH; i++)
		{
			PAGE_TABLE* table = tables + i;
			main_dir->SetFlag(i, flags);
			main_dir->SetTable(i, table);
		}

		//Map page dir
		MapPages(main_dir, main_dir, 1, flags);
		MapPages((void*)PAGE_DIR_ADDRESS, main_dir, 1, flags);

		//Map tables
		MapPages(tables, tables, PAGE_DIR_LENGTH, flags);
		MapPages((void*)PAGE_TABLE_ADDRESS(0), tables, PAGE_DIR_LENGTH - 1, flags);

		//Map kernel and memory map
		MapPages((void*)KERNEL_BASE, (void*)KERNEL_BASE, BYTES_TO_BLOCKS((size_t)&__KERNEL_END - (size_t)&__KERNEL_START + MEMORY_MAP_SIZE), flags);

        ADDRESS_SPACE addr_space;
        addr_space.ptr = main_dir;
        
		SwapAddressSpace(addr_space);
		EnablePaging();

        return 1;
    }
}