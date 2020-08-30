#include <Arch/memory.h>
#include <debug.h>
#include <string.h>

#define PAGE_DIR_INDEX(x)   ((size_t(x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) ((size_t(x) >> 12) & 0x3FF)

#define PAGE_DIR_ADDRESS          0xFFFFF000
#define FIRST_PAGE_ADDRESS        0xFFC00000
#define PAGE_TABLE_ADDRESS(index) (FIRST_PAGE_ADDRESS + ((index)) * PAGE_SIZE)
#define PAGE_TABLE_AT(virt)       (PAGE_TABLE *)(PAGE_TABLE_ADDRESS(PAGE_DIR_INDEX((virt))));

#define F_PF(flags)                                           \
    (pflags_t)((PAGE_PRESENT * (bool)((flags)&PTE_PRESENT)) | \
               (PAGE_WRITE * (bool)((flags)&PTE_WRITABLE)) |  \
               (PAGE_USER * (bool)((flags)&PTE_USER)))
#define PF_F(pflags)                                 \
    ((PTE_PRESENT * (bool)((pflags)&PAGE_PRESENT)) | \
     (PTE_WRITABLE * (bool)((pflags)&PAGE_WRITE)) | (PTE_USER * (bool)((pflags)&PAGE_USER)))

extern size_t __KERNEL_START, __KERNEL_END;

namespace VMem::Arch {
struct ADDRESS_SPACE_MAPPING
{
    PAGE_DIR *dir;
    PAGE_TABLE *tables;
    bool is_phys;

    ADDRESS_SPACE_MAPPING()
    {
        this->dir = 0;
        this->tables = 0;
        this->is_phys = false;
    }

    ~ADDRESS_SPACE_MAPPING()
    {
        /*if (this->dir)
                FreePages(this->dir, 1);

        if (this->tables)
                FreePages(this->tables, PAGE_DIR_LENGTH);*/
    }

    PAGE_TABLE *GetTable(size_t virt) const
    {
        int index = PAGE_DIR_INDEX(virt);

        if (this->is_phys)
            return dir->GetTable(index);

        return &tables[index];
    }

    size_t GetAddress(size_t virt) const
    {
        PAGE_TABLE *table = GetTable(virt);
        return (size_t)table->GetAddr(PAGE_TABLE_INDEX(virt));
    }

    uint32 GetPTFlags(size_t virt) const
    {
        PAGE_TABLE *table = GetTable(virt);
        return table->GetFlags(PAGE_TABLE_INDEX(virt));
    }

    pflags_t GetFlags(size_t virt) const { return F_PF(GetPTFlags(virt)); }
};

PAGE_DIR *main_dir;
ADDRESS_SPACE current_space;

void EnablePaging()
{
    asm volatile("mov %cr0, %eax\n"
                 "or $0x80000000, %eax\n"
                 "mov %eax, %cr0");
}

void *_FirstFree(size_t count, size_t start, size_t end, ADDRESS_SPACE_MAPPING *mapping)
{
    for (size_t i = start; i < end; i += PAGE_SIZE) {
        if ((mapping->GetPTFlags(i) == 0) && (mapping->GetAddress(i) == 0)) {
            bool found = true;

            for (size_t j = 1; j < count; j++) {
                if ((mapping->GetPTFlags(i + j * PAGE_SIZE) != 0) ||
                    (mapping->GetAddress(i + j * PAGE_SIZE) != 0)) {
                    found = false;
                    break;
                }
            }

            if (found) {
                return (void *)i;
            }
        }
    }

    return 0;
}

bool _MapPages(void *_virt, void *_phys, size_t count, pflags_t flags,
               ADDRESS_SPACE_MAPPING *mapping)
{
    size_t virt = (size_t)_virt;
    size_t phys = (size_t)_phys;

    PAGE_DIR *dir = mapping->dir;
    uint32 pt_flags = PF_F(flags);

    for (size_t i = 0; i < count; i++) {
        int dir_index = PAGE_DIR_INDEX(virt);
        int table_index = PAGE_TABLE_INDEX(virt);

        PAGE_TABLE *table = mapping->GetTable(virt);

        if (VMem::page_list) {
            PAGE_INFO *info = GetInfo(virt);

            if (table->GetAddr(table_index))
                info->refs -= 1;
        }

        table->SetFlags(table_index, pt_flags);
        table->SetAddr(table_index, (void *)phys);

        if (VMem::page_list) {
            PAGE_INFO *info = GetInfo(virt);

            if (phys)
                info->refs += 1;
        }

        // if (mapping == CurrentMapping())
        asm volatile("invlpg (%0)" ::"r"(virt));

        // asm volatile(
        //	"mov %cr3, %eax\n"
        //	"mov %eax, %cr3");

        phys += PAGE_SIZE;
        virt += PAGE_SIZE;
    }

    return true;
}

///

ADDRESS_SPACE GetAddressSpace()
{
    return current_space;
}

ADDRESS_SPACE_MAPPING CurrentMapping()
{
    ADDRESS_SPACE_MAPPING mapping;

    if (current_space.ptr) {
        mapping.dir = (PAGE_DIR *)PAGE_DIR_ADDRESS;
        mapping.tables = (PAGE_TABLE *)FIRST_PAGE_ADDRESS;
    } else {
        mapping.dir = main_dir;
        mapping.tables = 0;
        mapping.is_phys = true;
    }

    return mapping;
}

size_t GetAddress(size_t virt)
{
    return CurrentMapping().GetAddress(virt);
}

pflags_t GetFlags(size_t virt)
{
    return CurrentMapping().GetFlags(virt);
}

void *FirstFree(size_t count, size_t start, size_t end)
{
    ADDRESS_SPACE_MAPPING mapping = CurrentMapping();
    return _FirstFree(count, start, end, &mapping);
}

bool MapPages(void *virt, void *phys, size_t count, pflags_t flags)
{
    ADDRESS_SPACE_MAPPING mapping = CurrentMapping();
    return _MapPages(virt, phys, count, flags, &mapping);
}

bool CreateMapping(ADDRESS_SPACE &space, ADDRESS_SPACE_MAPPING *mapping)
{
    if (space.ptr == 0)
        return false;

    pflags_t flags = PAGE_PRESENT | PAGE_WRITE;

    mapping->dir = (PAGE_DIR *)KernelMapFirstFree(space.ptr, 1, flags);
    mapping->tables = (PAGE_TABLE *)FirstFree(PAGE_DIR_LENGTH, KERNEL_BASE, KERNEL_END);

    for (int i = 0; i < PAGE_DIR_LENGTH; i++) {
        void *phys_table = mapping->dir->GetTable(i);

        if (!MapPages(&mapping->tables[i], phys_table, 1, flags)) {
            // Todo: free memory
            return false;
        }
    }

    return true;
}

bool FreeMapping(ADDRESS_SPACE_MAPPING *mapping)
{
    MapPages(mapping->dir, 0, 1, PAGE_NONE);
    MapPages(mapping->tables, 0, PAGE_DIR_LENGTH, PAGE_NONE);
    return true;
}

bool CopyMemoryTo(ADDRESS_SPACE &to)
{
    ADDRESS_SPACE_MAPPING mapping;

    if (CreateMapping(to, &mapping)) {
        size_t cb = 0;

        // Count present pages
        for (size_t virt = USER_BASE; virt < USER_END; virt += PAGE_SIZE) {
            if (GetFlags(virt) != 0)
                cb++;
        }

        // Allocate physical memory and map temporary for copying data
        char *phys = (char *)PMem::AllocBlocks(cb);
        char *tmpvirt = (char *)KernelMapFirstFree(phys, cb, PAGE_PRESENT | PAGE_WRITE);
        char *copy = tmpvirt;

        for (size_t virt = USER_BASE; virt < USER_END; virt += PAGE_SIZE) {
            pflags_t pf = GetFlags(virt);

            if (pf != 0) {
                _MapPages((void *)virt, phys, 1, pf, &mapping);
                memcpy(copy, (void *)virt, PAGE_SIZE);

                phys += PAGE_SIZE;
                copy += PAGE_SIZE;
            }
        }

        FreePages(tmpvirt, cb);
        FreeMapping(&mapping);
        return true;
    }

    return false;
}

bool CopyOnWrite(void *virt1, void *virt2, size_t count, ADDRESS_SPACE &to)
{
    ADDRESS_SPACE_MAPPING current_mapping = CurrentMapping();
    ADDRESS_SPACE_MAPPING mapping;

    if (CreateMapping(to, &mapping)) {
        size_t virt_addr1 = (size_t)virt1;
        size_t virt_addr2 = (size_t)virt2;

        for (size_t i = 0; i < count; i++) {
            PAGE_INFO *info = GetInfo(virt_addr1);

            if (info->refs > 0) {
                pflags_t flags = GetFlags(virt_addr1);
                size_t phys = GetAddress(virt_addr1);
                pflags_t newflags = flags & ~PAGE_WRITE;

                if (info->cow == 0) {
                    info->cow = 2;
                    info->writable = flags & PAGE_WRITE;
                    _MapPages((void *)virt_addr1, (void *)phys, 1, newflags, &current_mapping);
                    _MapPages((void *)virt_addr2, (void *)phys, 1, newflags, &mapping);
                } else if (info->cow == 1) {
                    info->cow = 2;
                    _MapPages((void *)virt_addr2, (void *)phys, 1, newflags, &mapping);
                } else {
                    info->cow += 1;
                    _MapPages((void *)virt_addr2, (void *)phys, 1, newflags, &mapping);
                }
            }

            virt_addr1 += PAGE_SIZE;
            virt_addr2 += PAGE_SIZE;
        }

        FreeMapping(&mapping);
        return true;
    }

    return false;
}

bool SwapAddressSpace(ADDRESS_SPACE &addr_space)
{
    if (addr_space == current_space)
        return true;

    current_space = addr_space;

    asm volatile("mov (%0), %%eax\n"
                 "mov %%eax, %%cr3" ::"r"(&addr_space.ptr));

    return true;
}

bool CreateAddressSpace(ADDRESS_SPACE &space)
{
    pflags_t flags = PAGE_PRESENT | PAGE_WRITE;
    uint32 pt_flags = PF_F(flags);

    // Dir
    PAGE_DIR *dir_virt = (PAGE_DIR *)KernelAlloc(1);
    PAGE_DIR *dir_phys = (PAGE_DIR *)GetAddress((size_t)dir_virt);

    // Tables
    PAGE_TABLE *tables_virt = (PAGE_TABLE *)(KernelAlloc(768));
    PAGE_TABLE *tables_phys = (PAGE_TABLE *)GetAddress((size_t)tables_virt);
    memset(tables_virt, 0, sizeof(PAGE_TABLE) * 768);

    tables_phys -= 256;
    tables_virt -= 256;

    for (int i = 0; i < 256; i++) {
        dir_virt->values[i] = main_dir->values[i];
    }

    for (int i = 256; i < PAGE_DIR_LENGTH; i++) {
        dir_virt->SetFlags(i, PDE_PRESENT | PDE_WRITABLE | PDE_USER);
        dir_virt->SetTable(i, tables_phys + i);
    }

    // Map tables and directory to last table
    PAGE_TABLE *cur_last_table = main_dir->GetTable(PAGE_DIR_LENGTH - 1);
    PAGE_TABLE *last_table = &tables_virt[PAGE_DIR_LENGTH - 1];

    dir_virt->SetFlags(PAGE_DIR_LENGTH - 1, pt_flags);

    for (int i = 0; i < 256; i++) {
        last_table->values[i] = cur_last_table->values[i];
    }

    for (int i = 256; i < 1023; i++) {
        last_table->SetFlags(i, PDE_PRESENT | PDE_WRITABLE | PDE_USER);
        last_table->SetAddr(i, tables_phys + i);
    }

    last_table->SetFlags(PAGE_TABLE_LENGTH - 1, pt_flags);
    last_table->SetAddr(PAGE_TABLE_LENGTH - 1, dir_phys);

    MapPages(dir_virt, 0, 1, PAGE_NONE);
    MapPages(tables_virt + 256, 0, 768, PAGE_NONE);

    space.ptr = dir_phys;
    return true;
}

bool DestroyAddressSpace(ADDRESS_SPACE &space)
{
    // TODO
    return false;
}

bool CopyAddressSpace(ADDRESS_SPACE &space)
{
    if (!CreateAddressSpace(space))
        return false;

    // if (!CopyMemoryTo(space))
    //	return false;

    size_t blocks = (USER_END - USER_BASE) / PAGE_SIZE;
    if (!CopyOnWrite((void *)USER_BASE, (void *)USER_BASE, blocks, space))
        return false;

    return true;
}

bool Init()
{
    current_space.ptr = 0;

    // Page dir
    main_dir = (PAGE_DIR *)PMem::AllocBlocks(1);
    memset(main_dir, 0, sizeof(PAGE_DIR));

    // Tables
    PAGE_TABLE *tables = (PAGE_TABLE *)PMem::AllocBlocks(PAGE_DIR_LENGTH);
    memset(tables, 0, sizeof(PAGE_TABLE) * PAGE_DIR_LENGTH);

    pflags_t flags = PAGE_PRESENT | PAGE_WRITE;

    for (int i = 0; i < PAGE_DIR_LENGTH; i++) {
        PAGE_TABLE *table = tables + i;
        main_dir->SetFlags(i, flags);
        main_dir->SetTable(i, table);
    }

    // Map page dir
    MapPages(main_dir, main_dir, 1, flags);
    MapPages((void *)PAGE_DIR_ADDRESS, main_dir, 1, flags);

    // Map tables
    MapPages(tables, tables, PAGE_DIR_LENGTH, flags);
    MapPages((void *)PAGE_TABLE_ADDRESS(0), tables, PAGE_DIR_LENGTH - 1, flags);

    // Map kernel and memory map
    MapPages((void *)KERNEL_BASE, (void *)KERNEL_BASE,
             BYTES_TO_BLOCKS((size_t)&__KERNEL_END - (size_t)&__KERNEL_START + MEMORY_MAP_SIZE),
             flags);

    ADDRESS_SPACE addr_space;
    addr_space.ptr = main_dir;

    SwapAddressSpace(addr_space);
    EnablePaging();

    return true;
}
} // namespace VMem::Arch
