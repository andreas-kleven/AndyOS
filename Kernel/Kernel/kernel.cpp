#include "kernel.h"
#include "os.h"
#include "Arch/init.h"
#include "Boot/multiboot.h"
#include "Drawing/vbe.h"
#include "Drivers/serial.h"
#include "Drivers/ata.h"
#include "Drivers/mouse.h"
#include "Drivers/keyboard.h"
#include "Memory/memory.h"
#include "Process/scheduler.h"
#include "FS/vfs.h"
#include "task.h"
#include "syscalls.h"
#include "debug.h"

extern size_t __KERNEL_START, __KERNEL_END;

namespace Kernel
{
	MULTIBOOT_INFO boot_info;
	VBE_MODE_INFO vbe_mode;

	void Setup(MULTIBOOT_INFO* bootinfo)
	{
		boot_info = *bootinfo;
		vbe_mode = *(VBE_MODE_INFO*)boot_info.vbe_mode_info;

		size_t kernel_start = (size_t)&__KERNEL_START;
		size_t kernel_end = (size_t)&__KERNEL_END;
		size_t mem_end = boot_info.mem_upper * 0x400;
		size_t mem_size = mem_end;

		debug_init(1);
		debug_color(0xFF00FF00);
		debug_pos(0, 2);

		Arch::Init();

		PMem::Init(mem_size, (void*)kernel_end);
		PMem::InitRegion((void*)(kernel_end + MEMORY_MAP_SIZE), mem_end - kernel_end);
		VMem::Init();

		Syscalls::Init();

		VBE::Init(&vbe_mode);
		debug_print("Init VBE: %i %i %i\n", vbe_mode.width, vbe_mode.height, vbe_mode.bpp);

		DriverManager::Init();
		debug_print("Init devices\n");

		VFS::Init();
		debug_print("Init VFS\n");

		Keyboard::Init();
		debug_print("Init Keyboard\n");

		Scheduler::Init();
		debug_print("Init Scheduler\n");

		Task::Start(OS::Main);
	}
}