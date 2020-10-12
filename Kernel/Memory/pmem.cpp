#include <Process/scheduler.h>
#include <debug.h>
#include <hal.h>
#include <memory.h>
#include <string.h>

namespace PMem {
size_t mem_size;
uint32 *mem_map;

size_t num_blocks;
size_t num_free;

inline uint8 GetBit(size_t bit)
{
    return (mem_map[bit / 32] >> (bit % 32)) & 1;
}

inline void SetBit(size_t bit)
{
    if (GetBit(bit)) {
        kprintf("PMem SetBit error\n");
        sys_halt();
    }

    mem_map[bit / 32] |= (1 << (bit % 32));
    num_free--;
}

inline void UnsetBit(size_t bit)
{
    if (!GetBit(bit)) {
        kprintf("PMem UnsetBit error\n");
        sys_halt();
    }

    mem_map[bit / 32] &= ~(1 << (bit % 32));
    num_free++;
}

size_t FirstFree()
{
    Scheduler::Disable();

    for (size_t i = 1; i < num_blocks; i++) {
        if (!GetBit(i)) {
            Scheduler::Enable();
            return i;
        }
    }

    Scheduler::Enable();
    kprintf("Out of memory\n");
    return 0;
}

size_t FirstFreeNum(size_t size)
{
    if (size == 0)
        return 0;

    if (size == 1)
        return FirstFree();

    Scheduler::Disable();

    size_t i, j;

    for (i = 1; i <= num_blocks - size; i++) {
        if (!GetBit(i)) {
            bool found = true;

            for (j = 1; j < size; j++) {
                if (GetBit(i + j))
                    found = false;
            }

            if (found) {
                Scheduler::Enable();
                return i;
            } else {
                i += j;
            }
        }
    }

    kprintf("Out of memory %p %p\n\n", num_blocks, num_free);
    Scheduler::Enable();
    return 0;
}

size_t NumBlocks()
{
    return num_blocks;
}

size_t NumFree()
{
    return num_free;
}

size_t NumUsed()
{
    return num_blocks - num_free;
}

void InitRegion(void *addr, size_t size)
{
    int align = (size_t)addr / BLOCK_SIZE;
    int num = size / BLOCK_SIZE;

    if (align == 0) {
        align = 1;
        num -= 1;
    }

    for (int i = 0; i < num; i++)
        UnsetBit(align++);
}

void DeInitRegion(void *addr, size_t size)
{
    int align = (size_t)addr / BLOCK_SIZE;
    int blocks = size / BLOCK_SIZE;

    for (int i = 0; i < blocks; i++)
        SetBit(align++);
}

void *AllocBlocks(size_t size)
{
    Scheduler::Disable();
    size_t frame = FirstFreeNum(size);

    if (frame == 0) {
        Scheduler::Enable();
        return 0;
    }

    for (size_t i = 0; i < size; i++)
        SetBit(frame + i);

    Scheduler::Enable();
    return (void *)(frame * BLOCK_SIZE);
}

void FreeBlocks(void *addr, size_t size)
{
    int frame = (size_t)addr / BLOCK_SIZE;

    if (frame == 0 || size == 0)
        return;

    Scheduler::Disable();

    for (size_t i = 0; i < size; i++) {
        if (GetBit(frame + i))
            UnsetBit(frame + i);
    }

    Scheduler::Enable();
}

STATUS Init(size_t mem_end, void *map)
{
    mem_size = mem_end;
    mem_map = (uint32 *)map;

    num_blocks = BYTES_TO_BLOCKS(mem_size);
    num_free = num_blocks;

    memset(mem_map, 0, MEMORY_MAP_SIZE);
    DeInitRegion(0, mem_size);
    return STATUS_SUCCESS;
}
} // namespace PMem
