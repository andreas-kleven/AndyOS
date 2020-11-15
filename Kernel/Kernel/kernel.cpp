#include <Arch/init.h>
#include <Drivers/ata.h>
#include <Drivers/serial.h>
#include <FS/vfs.h>
#include <Kernel/task.h>
#include <Kernel/test.h>
#include <Process/dispatcher.h>
#include <Process/scheduler.h>
#include <debug.h>
#include <kernel.h>
#include <memory.h>
#include <syscalls.h>
#include <video.h>

extern size_t __KERNEL_START, __KERNEL_END;

namespace Kernel {
void Setup(size_t mem_size, size_t stack_phys, size_t stack_size, VideoMode *video_mode)
{
    size_t kernel_start = (size_t)&__KERNEL_START;
    size_t kernel_end = (size_t)&__KERNEL_END;
    size_t kernel_size = kernel_end - kernel_start;
    size_t memmap_end = kernel_end + MEMORY_MAP_SIZE;
    size_t pagedir_phys = KERNEL_PHYS + kernel_size + MEMORY_MAP_SIZE;
    size_t pagedir_virt = memmap_end;
    size_t kernel_reserved = kernel_size + MEMORY_MAP_SIZE + KERNEL_STACK_SIZE + VMEM_TABLES_SIZE;
    size_t kernel_phys_end = KERNEL_PHYS + kernel_reserved;

    // align
    pagedir_phys += (0x1000 - pagedir_phys % 0x1000) % 0x1000;
    pagedir_virt += (0x1000 - pagedir_virt % 0x1000) % 0x1000;

    debug_init(true, false);
    debug_color(0xFF00FF00);
    debug_pos(0, 2);

    Scheduler::PreInit();
    Arch::Init();

    PMem::Init(mem_size, (void *)kernel_end);
    PMem::InitRegion(kernel_phys_end, mem_size - kernel_phys_end);

    if (!VMem::Init(pagedir_phys, pagedir_virt, stack_phys, stack_size))
        return;

    Syscalls::Init();

    Video::SetMode(video_mode);
    kprintf("Init video: %i %i %i\n", Video::mode->width, Video::mode->height, Video::mode->depth);

    DriverManager::Init();
    kprintf("Init devices\n");

    VFS::Init();
    kprintf("Init VFS\n");

    Scheduler::Init();
    kprintf("Init Scheduler\n");

    Scheduler::Start(Test::Start);
}
} // namespace Kernel
