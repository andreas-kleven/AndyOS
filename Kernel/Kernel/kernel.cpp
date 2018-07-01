#include "kernel.h"
#include "os.h"
#include "API/syscalls.h"
#include "Boot/multiboot.h"
#include "Drawing/vbe.h"
#include "Drawing/drawing.h"
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

void Kernel::Setup(MULTIBOOT_INFO* bootinfo)
{
	Debug::Init(1);
	Debug::color = 0xFF00FF00;
	Debug::y = 2;

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

void Kernel::HigherHalf(MULTIBOOT_INFO bootinfo)
{
	VBE_MODE_INFO* vbeMode = (VBE_MODE_INFO*)bootinfo.vbe_mode_info;
	VBE::Init(vbeMode);
	Drawing::Init(VBE::mode.width, VBE::mode.height, VBE::mem_base);

	Debug::Print("Init VBE: %i %i %i\n", VBE::mode.width, VBE::mode.height, VBE::mode.bpp);

	DeviceManager::Init();
	Debug::Print("Init devices\n");

	VFS::Init();
	Debug::Print("Init VFS\n");

	Mouse::Init(Drawing::gc.width, Drawing::gc.height, 0.5);
	Debug::Print("Init Mouse\n");
	
	Keyboard::Init();
	Debug::Print("Init Keyboard\n");

	Scheduler::Init();
	Debug::Print("Init Scheduler\n");

	THREAD* mainThread = Scheduler::CreateKernelThread(OS::Main);
	Scheduler::InsertThread(mainThread);
	Scheduler::StartThreading();
}