#pragma once
#include <types.h>
#include <Process/process.h>

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

enum ELF32_SECTION_TYPE
{
	ELF32_SHT_NULL = 0x00,
	ELF32_SHT_PROGBITS = 0x01,
	ELF32_SHT_SYMTAB = 0x02,
	ELF32_SHT_STRTAB = 0x03,
	ELF32_SHT_RELA = 0x04,
	ELF32_SHT_HASH = 0x05,
	ELF32_SHT_DYNAMIC = 0x06,
	ELF32_SHT_NOTE = 0x07,
	ELF32_SHT_NOBITS = 0x08,
	ELF32_SHT_REL = 0x09,
	ELF32_SHT_SHLIB = 0x0A,
	ELF32_SHT_DYNSYM = 0x0B,
	ELF32_SHT_INIT_ARRAY = 0x0E,
	ELF32_SHT_FINI_ARRAY = 0x0F,
	ELF32_SHT_PREINIT_ARRAY = 0x010,
	ELF32_SHT_GROUP = 0x11,
	ELF32_SHT_SYMTAB_SHNDX = 0x12,
	ELF32_SHT_NUM = 0x13,
	ELF32_SHT_LOOS = 0x60000000
};

struct ELF32_SHEADER {
	uint32 sh_name;
	ELF32_SECTION_TYPE sh_type;
	uint32 sh_flags;
	uint32 sh_addr;
	uint32 sh_offset;
	uint32 sh_size;
	uint32 sh_link;
	uint32 sh_info;
	uint32 sh_addralign;
	uint32 sh_entsize;
} __attribute__((packed));

namespace ELF
{
    size_t Load(const char* path, PROCESS* proc);
};