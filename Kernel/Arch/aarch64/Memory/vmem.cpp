#include "Arch/memory.h"
#include "Memory/memory.h"
#include "types.h"

namespace VMem::Arch
{
    ADDRESS_SPACE GetAddressSpace()
    {
        return ADDRESS_SPACE();
    }

    bool SwapAddressSpace(ADDRESS_SPACE& space)
    {
        return 0;
    }

    ADDRESS_SPACE CreateAddressSpace()
    {
        return ADDRESS_SPACE();
    }

    size_t GetAddress(size_t virt)
    {
        return 0;
    }

    pflags_t GetFlags(size_t virt)
    {
        return 0;
    }

    void* FirstFree(size_t count, size_t start, size_t end)
    {
        return 0;
    }

    bool MapPages(void* virt, void* phys, size_t count, pflags_t flags)
    {
        return 0;
    }

    bool FreePages(void* virt, size_t count)
    {
        return 0;
    }

    bool Init()
    {
        return 0;
    }
}