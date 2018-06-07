#include "process.h"
#include "File System/filesys.h"
#include "HAL/hal.h"
#include "Memory/memory.h"
#include "scheduler.h"
#include "string.h"
#include "debug.h"

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#define IMAGE_SIZEOF_SHORT_NAME 8

struct DOS_Header
{
	// uint16 is 2 bytes, long is 4 bytes
	uint8 signature[2];
	uint16 lastsize;
	uint16 nblocks;
	uint16 nreloc;
	uint16 hdrsize;
	uint16 minalloc;
	uint16 maxalloc;
	uint16 ss; // 2 byte value
	uint16 sp; // 2 byte value
	uint16 checksum;
	uint16 ip; // 2 byte value
	uint16 cs; // 2 byte value
	uint16 relocpos;
	uint16 noverlay;
	uint16 reserved1[4];
	uint16 oem_id;
	uint16 oem_info;
	uint16 reserved2[10];
	uint32  e_lfanew; // Offset to the 'PE\0\0' signature relative to the beginning of the file
};

typedef struct _IMAGE_DATA_DIRECTORY {
	uint32 VirtualAddress;
	uint32 Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER {
	uint16                 Magic;
	uint8                 MajorLinkerVersion;
	uint8                 MinorLinkerVersion;
	uint32                SizeOfCode;
	uint32                SizeOfInitializedData;
	uint32                SizeOfUninitializedData;
	uint32                AddressOfEntryPoint;
	uint32                BaseOfCode;
	uint32                BaseOfData;
	uint32                ImageBase;
	uint32                SectionAlignment;
	uint32                FileAlignment;
	uint16                 MajorOperatingSystemVersion;
	uint16                 MinorOperatingSystemVersion;
	uint16                 MajorImageVersion;
	uint16                 MinorImageVersion;
	uint16                 MajorSubsystemVersion;
	uint16                 MinorSubsystemVersion;
	uint32                Win32VersionValue;
	uint32                SizeOfImage;
	uint32                SizeOfHeaders;
	uint32                CheckSum;
	uint16                 Subsystem;
	uint16                 DllCharacteristics;
	uint32                SizeOfStackReserve;
	uint32                SizeOfStackCommit;
	uint32                SizeOfHeapReserve;
	uint32                SizeOfHeapCommit;
	uint32                LoaderFlags;
	uint32                NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_FILE_HEADER {
	uint16  Machine;
	uint16  NumberOfSections;
	uint32 TimeDateStamp;
	uint32 PointerToSymbolTable;
	uint32 NumberOfSymbols;
	uint16  SizeOfOptionalHeader;
	uint16  Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;


struct IMAGE_NT_HEADERS {
	uint32                 Signature;
	IMAGE_FILE_HEADER     FileHeader;
	IMAGE_OPTIONAL_HEADER OptionalHeader;
};

typedef struct _IMAGE_SECTION_HEADER {
	uint8  Name[IMAGE_SIZEOF_SHORT_NAME];
	union {
		uint32 PhysicalAddress;
		uint32 VirtualSize;
	} Misc;
	uint32 VirtualAddress;
	uint32 SizeOfRawData;
	uint32 PointerToRawData;
	uint32 PointerToRelocations;
	uint32 PointerToLinenumbers;
	uint16  NumberOfRelocations;
	uint16  NumberOfLinenumbers;
	uint32 Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

PROCESS_INFO* Process::Create(char* filename)
{
	char* image;
	int size = FS::ReadFile(filename, image);

	if (!size)
		return 0;

	DOS_Header* dos_header = (DOS_Header*)image;
	IMAGE_NT_HEADERS* ntheader = (IMAGE_NT_HEADERS*)(uint32(image) + dos_header->e_lfanew);

	uint32 entryAddress = ntheader->OptionalHeader.AddressOfEntryPoint;
	uint32 imagebase = ntheader->OptionalHeader.ImageBase;
	uint32 codebase = ntheader->OptionalHeader.BaseOfCode;
	uint32 alignment = ntheader->OptionalHeader.FileAlignment;

	IMAGE_SECTION_HEADER* textheader = 0;

	uint32 flags = PTE_PRESENT | PTE_WRITABLE | PTE_USER;

	_asm cli

	PAGE_DIR* dir = VMem::CreatePageDir();
	VMem::SwitchDir(dir);

	for (int i = 0; i < ntheader->FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER* sectionHeader = (IMAGE_SECTION_HEADER*)((uint32)image + dos_header->e_lfanew + 248 + (i * 40));

		if (memcmp(sectionHeader->Name, ".text", 5) == 0)
		{
			textheader = sectionHeader;
		}

		uint32 blocks = BYTES_TO_BLOCKS(sectionHeader->SizeOfRawData);

		uint32 phys = (uint32)PMem::AllocBlocks(blocks);
		uint32 virt = imagebase + sectionHeader->VirtualAddress;

		VMem::MapPhysAddr(dir, phys, virt, flags, blocks);

		PAGE_TABLE_ENTRY* e = VMem::GetTableEntry(dir, virt);
		memcpy((uint32*)virt, image + sectionHeader->PointerToRawData, sectionHeader->SizeOfRawData);
	}

	uint32 pointerToRawData = textheader->PointerToRawData;
	int entry = imagebase + entryAddress;

	Debug::Print("Loaded image %ux\n", dir);

	PROCESS_INFO* proc = new PROCESS_INFO(PROCESS_USER, dir);
	Process::CreateThread(proc, (void*)entry);

	_asm sti
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

THREAD* Process::CreateThread(PROCESS_INFO* proc, void* main)
{
	THREAD* thread = 0;

	//Create thread
	switch (proc->flags)
	{
	case PROCESS_KERNEL:
		thread = Scheduler::CreateKernelThread(main);
		break;

	case PROCESS_USER:
		uint32 stackPhys = (uint32)PMem::AllocBlocks(1);
		uint8* stack = (uint8*)VMem::UserMapFirstFree(proc->page_dir, stackPhys, PTE_PRESENT | PTE_WRITABLE | PTE_USER, 1);
		VMem::MapPhysAddr(proc->page_dir, stackPhys, (uint32)stack, PTE_PRESENT | PTE_WRITABLE | PTE_USER, 1);

		thread = Scheduler::CreateUserThread(main, stack + BLOCK_SIZE);
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
