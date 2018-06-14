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

void Kernel::Setup(MULTIBOOT_INFO* bootinfo)
{
	Debug::Init(1);
	Debug::color = 0xFF00FF00;

	CPU::Init();
	HAL::Init();
	Exceptions::Init();
	Syscalls::Init();
	
	uint32 mem_map = KERNEL_BASE_PHYS + KERNEL_SIZE;
	PMem::Init(MEMORY_SIZE, (uint32*)mem_map);
	PMem::InitRegion((uint32*)0x30000000, 0xD0000000);
	VMem::Init(bootinfo);
}

void Kernel::HigherHalf(MULTIBOOT_INFO* bootinfo)
{
	VBE_MODE_INFO* vbeMode = (VBE_MODE_INFO*)bootinfo->vbe_mode_info;
	VBE::Init(vbeMode);
	Drawing::Init(VBE::mode.width, VBE::mode.height, VBE::mem_base);

	DeviceManager::Init();
	VFS::Init();

	Mouse::Init(Drawing::gc.width, Drawing::gc.height, 0.5);
	Keyboard::Init();

	Scheduler::Init();

	THREAD* mainThread = Scheduler::CreateKernelThread(OS::Main);
	Scheduler::InsertThread(mainThread);
	Scheduler::StartThreading();
}