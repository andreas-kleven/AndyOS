#include "elf.h"
#include "FS/vfs.h"
#include "Lib/debug.h"
#include "string.h"

PROCESS* ELF::Load(char* path)
{
	char* image;
	int size = VFS::ReadFile(path, image);

	if (!size)
	{
		Debug::Print("File not found '%s'", path);
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
	
	PROCESS* proc = new PROCESS(PROCESS_USER, dir);
	ProcessManager::CreateThread(proc, (void(*)())header->e_entry);

	asm volatile("sti");
	return proc;
}