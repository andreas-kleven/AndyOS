#include "process.h"
#include "File System/filesys.h"
#include "HAL/hal.h"
#include "Memory/memory.h"
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

PAGE_DIR* CreateAddressSpace()
{
	PAGE_DIR* dir = new PAGE_DIR;
	memset(dir, 0, sizeof(PAGE_DIR));

	//Tables
	PAGE_TABLE* tables = new PAGE_TABLE[PAGE_DIR_LENGTH];
	memset(tables, 0, sizeof(PAGE_TABLE) * PAGE_DIR_LENGTH);

	PAGE_DIR* current = VMem::GetCurrentDir();

	for (int i = 0; i < 256; i++)
	{
		dir->entries[i] = current->entries[i];
	}

	for (int i = 256; i < PAGE_DIR_LENGTH; i++)
	{
		PAGE_TABLE* table = tables + i;
		dir->entries[i].SetTable(table);
	}

	//Map framebuffer
	VMem::MapPhysAddr(dir, 0xE0000000, 0xE0000000, PTE_PRESENT | PTE_WRITABLE | PTE_USER, 0x300);
	return dir;
}

void Process::Create(char* filename)
{
	char* image;
	int size = FS::ReadFile(filename, image);

	if (!size)
		return;

	Debug::Dump(image, 256);

	DOS_Header* dos_header = (DOS_Header*)image;
	IMAGE_NT_HEADERS* ntheader = (IMAGE_NT_HEADERS*)(uint32(image) + dos_header->e_lfanew);

	uint32 entry = ntheader->OptionalHeader.AddressOfEntryPoint;
	uint32 imagebase = ntheader->OptionalHeader.ImageBase;
	uint32 codebase = ntheader->OptionalHeader.BaseOfCode;
	uint32 alignment = ntheader->OptionalHeader.FileAlignment;

	IMAGE_SECTION_HEADER* textheader = 0;

	uint32 flags = PTE_PRESENT | PTE_WRITABLE | PTE_USER;

	PAGE_DIR* dir = CreateAddressSpace();
	VMem::SwitchDir(dir);

	for (int i = 0; i < ntheader->FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER* sectionHeader = (IMAGE_SECTION_HEADER*)((uint32)image + dos_header->e_lfanew + 248 + (i * 40));

		if (memcmp(sectionHeader->Name, ".text", 5) == 0)
		{
			textheader = sectionHeader;
		}

		uint32* ptr = (uint32*)PMem::AllocBlocks(1);
		VMem::MapPhysAddr(dir, (uint32)ptr, (uint32)(imagebase + sectionHeader->VirtualAddress), flags);
		memcpy((uint32*)(imagebase + sectionHeader->VirtualAddress), image + sectionHeader->PointerToRawData, sectionHeader->SizeOfRawData);

		Debug::Print("%ux\t", sectionHeader->VirtualAddress);
		Debug::Dump(sectionHeader->Name, 8, true);
	}

	uint32 pointerToRawData = textheader->PointerToRawData;

	Debug::Print("Raw data: %ux\n", pointerToRawData);
	Debug::Print("Entry: %ux\n", entry);

	int absEntry = imagebase + entry;

	Debug::Print("Entry: %ux\n", absEntry);
	Debug::Print("End: %ux\n", ntheader->OptionalHeader.ImageBase + ntheader->OptionalHeader.SizeOfImage);

	uint32 stack = (uint32)PMem::AllocBlocks(1);
	uint8* stackVirt = (uint8*)(ntheader->OptionalHeader.ImageBase + ntheader->OptionalHeader.SizeOfImage + PAGE_SIZE);
	VMem::MapPhysAddr(dir, stack, (uint32)stackVirt - PAGE_SIZE, flags, 1);

	char* kstack = new char[BLOCK_SIZE];
	TSS::SetStack(KERNEL_SS, (uint32)kstack + BLOCK_SIZE);

	_asm
	{
		cli
		mov ax, 0x23; user mode data selector is 0x20 (GDT entry 3).Also sets RPL to 3
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax

		push 0x23; SS, notice it uses same selector as above
		push[stackVirt]; ESP
		//push esp

		//push 0x200
		pushfd; EFLAGS

		pop eax
		or eax, 0x200; enable IF in EFLAGS
		push eax

		push 0x1B; CS, user mode code selector is 0x18. With RPL 3 this is 0x1b
		push absEntry
		iretd
	}

	return;
}
