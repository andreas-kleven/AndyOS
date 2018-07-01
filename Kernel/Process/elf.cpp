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
		debug_print("File not found '%s'", path);
		return 0;
	}

	ELF32_HEADER* header = (ELF32_HEADER*)image;

	char sig[] = { 0x7F, 'E', 'L', 'F' };

	if (memcmp(&header->e_ident, &sig, sizeof(sig)))
	{
		//Not elf file
		debug_print("Invalid signature\n");
		return 0;
	}

	asm volatile("cli");

	PAGE_DIR* old_dir = VMem::GetCurrentDir();
	PAGE_DIR* dir = VMem::CreatePageDir();
	VMem::SwitchDir(dir);

	uint32 flags = PTE_PRESENT | PTE_WRITABLE | PTE_USER;

	for (int i = 0; i < header->e_phnum; i++)
	{
		ELF32_PHEADER* pheader = (ELF32_PHEADER*)(image + header->e_phoff + i * header->e_phentsize);

		uint32 blocks = BYTES_TO_BLOCKS(pheader->p_memsz) + 1;

		uint32 phys = (uint32)PMem::AllocBlocks(blocks);
		uint32 virt = pheader->p_vaddr;

		VMem::MapPhysAddr(phys, virt, flags, blocks);
		memcpy((uint32*)virt, image + pheader->p_offset, pheader->p_memsz);
	}

	for (int i = 0; i < header->e_shnum; i++)
	{
		ELF32_SHEADER* sheader = (ELF32_SHEADER*)(image + header->e_shoff + i * header->e_shentsize);

		if (sheader->sh_type == ELF32_SHT_NOBITS)
		{
			memset((void*)sheader->sh_addr, 0, sheader->sh_size);
		}
	}

	debug_print("Loaded image %ux\n", dir);
	
	PROCESS* proc = new PROCESS(PROCESS_USER, dir);
	ProcessManager::CreateThread(proc, (void(*)())header->e_entry);

	VMem::SwitchDir(old_dir);

	asm volatile("sti");
	return proc;
}