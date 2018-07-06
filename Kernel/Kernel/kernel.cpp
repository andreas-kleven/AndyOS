#include "kernel.h"
#include "os.h"
#include "API/syscalls.h"
#include "Boot/multiboot.h"
#include "Drawing/vbe.h"
#include "Drivers/serial.h"
#include "Drivers/ata.h"
#include "Drivers/mouse.h"
#include "Drivers/keyboard.h"
#include "exceptions.h"
#include "HAL/cpu.h"
#include "Memory/memory.h"
#include "Process/scheduler.h"
#include "FS/vfs.h"
#include "Lib/debug.h"

extern int __KERNEL_START, __KERNEL_END;

namespace Kernel
{
	void Setup(MULTIBOOT_INFO* bootinfo)
	{
		debug_init(1);
		debug_color(0xFF00FF00);
		debug_pos(0, 2);

		uint32 kernel_start = (uint32)&__KERNEL_START;
		uint32 kernel_end = (uint32)&__KERNEL_END;
		uint32 mem_end = bootinfo->mem_upper * 0x400;
		uint32 mem_size = mem_end;

		CPU::Init();
		HAL::Init();
		Exceptions::Init();
		Syscalls::Init();
		
		PMem::Init(mem_size, (uint32*)kernel_end);
		PMem::InitRegion((uint32*)(kernel_end + MEMORY_MAP_SIZE), mem_end - kernel_end);
		VMem::Init(bootinfo, kernel_end);
	}

	void HigherHalf(MULTIBOOT_INFO bootinfo)
	{
		VBE_MODE_INFO* vbeMode = (VBE_MODE_INFO*)bootinfo.vbe_mode_info;
		VBE::Init(vbeMode);

		debug_print("Init VBE: %i %i %i\n", vbeMode->width, vbeMode->height, vbeMode->bpp);

		DeviceManager::Init();
		debug_print("Init devices\n");

		VFS::Init();
		debug_print("Init VFS\n");

		Keyboard::Init();
		debug_print("Init Keyboard\n");

		Scheduler::Init();
		debug_print("Init Scheduler\n");

		THREAD* mainThread = Scheduler::CreateKernelThread(OS::Main);
		Scheduler::InsertThread(mainThread);
		Scheduler::StartThreading();
	}
}