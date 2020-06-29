#include "kernel.h"
#include "test.h"
#include "Arch/init.h"
#include "Drivers/serial.h"
#include "Drivers/ata.h"
#include "Drivers/mouse.h"
#include "Drivers/keyboard.h"
#include "Memory/memory.h"
#include "Process/scheduler.h"
#include "Process/dispatcher.h"
#include "FS/vfs.h"
#include "video.h"
#include "task.h"
#include "syscalls.h"
#include "debug.h"

extern size_t __KERNEL_START, __KERNEL_END;

namespace Kernel
{
	void Setup(size_t mem_start, size_t mem_end, VideoMode *video_mode)
	{
		size_t kernel_start = (size_t)&__KERNEL_START;
		size_t kernel_end = (size_t)&__KERNEL_END;

		debug_init(true);
		debug_color(0xFF00FF00);
		debug_pos(0, 2);

		Arch::Init();

		PMem::Init(mem_start, mem_end, (void *)kernel_end);
		PMem::InitRegion((void *)(kernel_end + MEMORY_MAP_SIZE), mem_end - kernel_end - MEMORY_MAP_SIZE);
		VMem::Init();

		Syscalls::Init();

		Video::SetMode(video_mode);
		debug_print("Init video: %i %i %i\n", Video::mode->width, Video::mode->height, Video::mode->depth);

		DriverManager::Init();
		debug_print("Init devices\n");

		VFS::Init();
		debug_print("Init VFS\n");

		Keyboard::Init();
		debug_print("Init Keyboard\n");

		Scheduler::Init();
		debug_print("Init Scheduler\n");

		Scheduler::Start(Test::Start);
	}
} // namespace Kernel
