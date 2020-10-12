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
void Setup(size_t mem_start, size_t mem_end, VideoMode *video_mode)
{
    size_t kernel_start = (size_t)&__KERNEL_START;
    size_t kernel_end = (size_t)&__KERNEL_END;

    debug_init(true, false);
    debug_color(0xFF00FF00);
    debug_pos(0, 2);

    Arch::Init();

    PMem::Init(mem_end, (void *)kernel_end);
    PMem::InitRegion((void *)(kernel_end + MEMORY_MAP_SIZE),
                     mem_end - kernel_end - MEMORY_MAP_SIZE);
    VMem::Init();

    Syscalls::Init();

    Video::SetMode(video_mode);
    kprintf("Init video: %i %i %i\n", Video::mode->width, Video::mode->height,
                Video::mode->depth);

    DriverManager::Init();
    kprintf("Init devices\n");

    VFS::Init();
    kprintf("Init VFS\n");

    Scheduler::Init();
    kprintf("Init Scheduler\n");

    Scheduler::Start(Test::Start);
}
} // namespace Kernel
