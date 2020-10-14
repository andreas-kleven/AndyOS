#include <Arch/memory.h>
#include <Process/scheduler.h>
#include <debug.h>
#include <kernel.h>
#include <memory.h>
#include <string.h>

namespace VMem {
PAGE_INFO *page_list = 0;

PAGE_INFO *GetInfo(size_t virt)
{
    size_t block = GetAddress(virt) / PAGE_SIZE;
    return &page_list[block];
}

void *MapFirstFree(void *phys, size_t count, pflags_t flags, size_t start, size_t end)
{
    Scheduler::Disable();
    void *virt = FirstFree(count, start, end);

    if (!virt) {
        Scheduler::Enable();
        return 0;
    }

    if (MapPages(virt, phys, count, flags)) {
        Scheduler::Enable();
        return (void *)virt;
    }

    Scheduler::Enable();
    return 0;
}

void *Alloc(size_t count, pflags_t flags, size_t start, size_t end, bool contiguous)
{
    Scheduler::Disable();

    if (contiguous || true) {
        void *virt = FirstFree(count, start, end);

        if (virt == 0) {
            Scheduler::Enable();
            return 0;
        }

        void *phys = PMem::AllocBlocks(count);

        if (!phys) {
            Scheduler::Enable();
            return 0;
        }

        if (!MapPages(virt, phys, count, flags)) {
            PMem::FreeBlocks(phys, count);
            Scheduler::Enable();
            return 0;
        }

        Scheduler::Enable();
        return virt;
    }
}

///

size_t GetAddress(size_t virt)
{
    Scheduler::Disable();
    size_t ret = Arch::GetAddress(virt);
    Scheduler::Enable();
    return ret;
}

pflags_t GetFlags(size_t virt)
{
    Scheduler::Disable();
    pflags_t ret = Arch::GetFlags(virt);
    Scheduler::Enable();
    return ret;
}

ADDRESS_SPACE GetAddressSpace()
{
    return Arch::GetAddressSpace();
}

bool SwapAddressSpace(ADDRESS_SPACE &space)
{
    Scheduler::Disable();
    bool ret = Arch::SwapAddressSpace(space);
    Scheduler::Enable();
    return ret;
}

bool CreateAddressSpace(ADDRESS_SPACE &space)
{
    Scheduler::Disable();
    bool ret = Arch::CreateAddressSpace(space);
    Scheduler::Enable();
    return ret;
}

bool DestroyAddressSpace(ADDRESS_SPACE &space)
{
    Scheduler::Disable();
    bool ret = Arch::DestroyAddressSpace(space);
    Scheduler::Enable();
    return ret;
}

bool CopyAddressSpace(ADDRESS_SPACE &space)
{
    Scheduler::Disable();
    bool ret = Arch::CopyAddressSpace(space);
    Scheduler::Enable();
    return ret;
}

bool CopyOnWrite(void *virt1, void *virt2, size_t count, ADDRESS_SPACE &to)
{
    Scheduler::Disable();
    bool ret = Arch::CopyOnWrite(virt1, virt2, count, to);
    Scheduler::Enable();
    return ret;
}

bool PerformCopy(void *virt)
{
    Scheduler::Disable();

    virt = (void *)(((size_t)virt / PAGE_SIZE) * PAGE_SIZE);

    PAGE_INFO *info = GetInfo((size_t)virt);
    size_t phys = GetAddress((size_t)virt);
    pflags_t flags = GetFlags((size_t)virt);
    pflags_t newflags = flags | PAGE_WRITE;

    kprintf("Copy %p cow=%d\n", virt, info->cow);

    if (info->cow == 0)
        return false;

    if (info->cow == 1) {
        info->cow = 0;

        if (!MapPages(virt, (void *)phys, 1, newflags)) {
            Scheduler::Enable();
            return false;
        }
    } else {
        void *copy = KernelAlloc(1);
        size_t newphys = GetAddress((size_t)copy);

        if (!copy) {
            Scheduler::Disable();
            return false;
        }

        memcpy(copy, virt, PAGE_SIZE);

        if (!MapPages(virt, (void *)newphys, 1, newflags)) {
            FreePages(copy, 1);
            Scheduler::Disable();
            return false;
        }

        if (!info->writable && info->cow == 2)
            info->cow = 0;
        else
            info->cow -= 1;

        FreePages(copy, 1);
    }

    Scheduler::Enable();
    return true;
}

void *FirstFree(size_t count, size_t start, size_t end)
{
    Scheduler::Disable();

    void *ret = Arch::FirstFree(count, start, end);

    if (ret == 0)
        kprintf("Out of virtual memory %p %p %p\n", count, start, end);

    Scheduler::Enable();
    return ret;
}

bool MapPages(void *virt, void *phys, size_t count, pflags_t flags)
{
    Scheduler::Disable();

    if (flags != PAGE_NONE && (virt == 0 || phys == 0)) {
        kprintf("Map address 0: %d, %p -> %p\n", virt, phys, flags);
        panic("", "");
        return false;
    }

    size_t virt_addr = (size_t)virt;
    size_t phys_addr = (size_t)phys;

    for (size_t i = 0; i < count; i++) {
        if (flags != PAGE_NONE)
            if ((flags != PAGE_NONE && GetFlags(virt_addr) != PAGE_NONE) ||
                (flags == PAGE_NONE && GetFlags(virt_addr) == PAGE_NONE))
                kprintf("Page already mapped %p, (%p, %p -> %p), (%p, %p -> %p)\n",
                        GetAddressSpace().ptr, flags, virt_addr, phys_addr, GetFlags(virt_addr),
                        virt_addr, GetAddress(virt_addr));

        if (!Arch::MapPages((void *)virt_addr, (void *)phys_addr, 1, flags)) {
            kprintf("Map address error: %d, %p -> %p\n", virt, phys, flags);
            panic("", "");
        }

        virt_addr += PAGE_SIZE;

        if (phys_addr)
            phys_addr += PAGE_SIZE;
    }

    Scheduler::Enable();
    return true;
}

bool FreePages(void *virt, size_t count)
{
    Scheduler::Disable();

    for (size_t addr = (size_t)virt; addr < (size_t)virt + count * PAGE_SIZE; addr += PAGE_SIZE) {
        PAGE_INFO *info = GetInfo(addr);

        if (info->refs == 1) {
            size_t phys = GetAddress(addr);
            PMem::FreeBlocks((void *)phys, 1);
        }

        // TODO: Move
        if (info->cow > 0)
            info->cow -= 1;
    }

    bool ret = MapPages(virt, 0, count, PAGE_NONE);

    Scheduler::Enable();
    return ret;
}

void *KernelAlloc(size_t count, bool contiguous)
{
    return Alloc(count, PAGE_PRESENT | PAGE_WRITE, KERNEL_BASE, KERNEL_END, contiguous);
}

void *UserAlloc(size_t count, bool contiguous)
{
    return Alloc(count, PAGE_PRESENT | PAGE_WRITE | PAGE_USER, HEAP_END, USER_END, contiguous);
}

void *KernelMapFirstFree(void *phys, size_t count, pflags_t flags)
{
    return MapFirstFree(phys, count, flags, KERNEL_BASE, KERNEL_END);
}

void *UserMapFirstFree(void *phys, size_t count, pflags_t flags)
{
    return MapFirstFree(phys, count, flags, HEAP_END, USER_END);
}

bool UserAllocShared(ADDRESS_SPACE other_space, void *&addr1, void *&addr2, size_t count)
{
    Scheduler::Disable();
    ADDRESS_SPACE cur_space = GetAddressSpace();

    void *phys = PMem::AllocBlocks(count);
    pflags_t flags = PAGE_PRESENT | PAGE_WRITE | PAGE_USER;

    addr1 = UserMapFirstFree(phys, count, flags);

    SwapAddressSpace(other_space);
    void *_addr2 = UserMapFirstFree(phys, count, flags);

    // Switch back
    SwapAddressSpace(cur_space);
    addr2 = _addr2;

    Scheduler::Enable();
    return true;
}

void InitPageList()
{
    page_list = new PAGE_INFO[MAX_BLOCKS];
    memset(page_list, 0, sizeof(PAGE_INFO) * MAX_BLOCKS);
    size_t virt = 0;

    while (true) {
        if (GetFlags(virt) & PAGE_PRESENT) {
            PAGE_INFO *info = GetInfo(virt);
            info->refs += 1;
        }

        if (virt == MEMORY_END)
            break;

        virt += PAGE_SIZE;
    }
}

bool Init(size_t dir_phys, size_t dir_virt, size_t stack_phys, size_t stack_size)
{
    if (!Arch::Init(dir_phys, dir_virt, stack_phys, stack_size))
        return false;

    InitPageList();

    return true;
}
} // namespace VMem
