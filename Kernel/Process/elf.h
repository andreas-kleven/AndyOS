#pragma once
#include "definitions.h"
#include "process.h"

struct ELF32_HEADER
{
	uint8  e_ident[16];
	uint16 e_type;
	uint16 e_machine;
	uint32 e_version;
	uint32 e_entry;
	uint32 e_phoff;
	uint32 e_shoff;
	uint32 e_flags;
	uint16 e_ehsize;
	uint16 e_phentsize;
	uint16 e_phnum;
	uint16 e_shentsize;
	uint16 e_shnum;
	uint16 e_shstrndx;
} __attribute__((packed));

struct ELF32_PHEADER
{
	uint32 p_type;
	uint32 p_offset;
	uint32 p_vaddr;
	uint32 p_paddr;
	uint32 p_filesz;
	uint32 p_memsz;
	uint32 p_flags;
	uint32 p_align;
} __attribute__((packed));

struct ELF32_SHEADER {
	uint32 sh_name;
	uint32 sh_type;
	uint32 sh_flags;
	uint32 sh_addr;
	uint32 sh_offset;
	uint32 sh_size;
	uint32 sh_link;
	uint32 sh_info;
	uint32 sh_addralign;
	uint32 sh_entsize;
} __attribute__((packed));

class ELF
{
public:
    static PROCESS* Load(char* path);
};