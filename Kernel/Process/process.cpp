#include "process.h"
#include "FS/vfs.h"
#include "HAL/hal.h"
#include "Memory/memory.h"
#include "scheduler.h"
#include "string.h"
#include "Lib/debug.h"

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#define IMAGE_SIZEOF_SHORT_NAME 8

PROCESS_INFO* Process::Create(char* filename)
{
	char* image;
	int size = VFS::ReadFile(filename, image);

	if (!size)
	{
		Debug::Print("Not found");
		return 0;
	}

	ELF32_HEADER* header = (ELF32_HEADER*)image;

	char sig[] = { 0x7F, 'E', 'L', 'F' };

	if (memcmp(&header->e_ident, &sig, sizeof(sig)))
	{
		//Not elf file
		Debug::Print("Invalid signature\n");
		return 0;
	}

	ELF32_PHEADER* pheader = (ELF32_PHEADER*)(image + header->e_phoff);
	ELF32_SHEADER* sheader = (ELF32_SHEADER*)(image + header->e_shoff);

	asm volatile("cli");

	PAGE_DIR* dir = VMem::CreatePageDir();
	VMem::SwitchDir(dir);

	uint32 flags = PTE_PRESENT | PTE_WRITABLE | PTE_USER;
	uint32 blocks = BYTES_TO_BLOCKS(pheader->p_memsz);

	uint32 phys = (uint32)PMem::AllocBlocks(blocks);
	uint32 virt = pheader->p_vaddr;

	VMem::MapPhysAddr(phys, virt, flags, blocks);

	memcpy((uint32*)virt, image + pheader->p_offset, pheader->p_memsz);

	Debug::Print("Loaded image %ux\n", dir);
	
	PROCESS_INFO* proc = new PROCESS_INFO(PROCESS_USER, dir);
	Process::CreateThread(proc, (void(*)())header->e_entry);

	asm volatile("sti");
	return proc;
}

STATUS Process::Terminate(PROCESS_INFO* proc)
{
	return STATUS_FAILED;
}

STATUS Process::Kill(PROCESS_INFO* proc)
{
	return STATUS_FAILED;
}

THREAD* Process::CreateThread(PROCESS_INFO* proc, void(*main)())
{
	THREAD* thread = 0;

	//Create thread
	switch (proc->flags)
	{
	case PROCESS_KERNEL:
		thread = Scheduler::CreateKernelThread(main);
		break;

	case PROCESS_USER:
		VMem::SwitchDir(proc->page_dir);

		uint32 stackPhys = (uint32)PMem::AllocBlocks(2);
		uint8* stack = (uint8*)VMem::UserMapFirstFree(stackPhys, PTE_PRESENT | PTE_WRITABLE | PTE_USER, 2);

		thread = Scheduler::CreateUserThread(main, stack + BLOCK_SIZE);
		Debug::Print("Stack: %ux\n", stack + BLOCK_SIZE);
		break;
	}

	if (!thread)
		return 0;

	thread->page_dir = proc->page_dir;

	//Insert into thread list
	if (proc->main_thread == 0)
	{
		thread->procNext = thread;
		proc->main_thread = thread;
	}
	else
	{
		thread->procNext = proc->main_thread->procNext;
		proc->main_thread->procNext = thread;
	}

	Scheduler::InsertThread(thread);
	return thread;
}

STATUS Process::RemoveThread(THREAD* thread)
{
	return STATUS_FAILED;
}

int procIdCounter = 0;

PROCESS_INFO::PROCESS_INFO(PROCESS_FLAGS flags, PAGE_DIR* page_dir)
{
	this->id = ++procIdCounter;
	this->flags = flags;
	this->page_dir = page_dir;
	this->next = 0;
	this->main_thread = 0;
}
