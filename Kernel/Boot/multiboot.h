#pragma once
#include "types.h"

#define MULTIBOOT_HEADER_MAGIC		0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC	0x2BADB002

#define MULTIBOOT_HEADER_FLAGS      0x00010004
#define STACK_SIZE                  0x4000
#define CHECKSUM					-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

#define CHECK_FLAG(flags,bit)       ((flags) & (1 << (bit)))

/* The Multiboot header. */
typedef struct MULTIBOOT_HEADER
{
	uint32 magic;
	uint32 flags;
	uint32 checksum;
	uint32 header_addr;
	uint32 load_addr;
	uint32 load_end_addr;
	uint32 bss_end_addr;
	uint32 entry_addr;
	uint32 mode_type;
	uint32 width;
	uint32 height;
	uint32 depth;
} __attribute__((packed)) MULTIBOOT_HEADER_T;


/* The symbol table for a.out. */
typedef struct AOUT_SYMBOL_TABLE
{
	uint32 tabsize;
	uint32 strsize;
	uint32 addr;
	uint32 reserved;
} __attribute__((packed)) AOUT_SYMBOL_TABLE_T;

/* The section header table for ELF. */
typedef struct ELF_SECTION_HEADER_TABLE
{
	uint32 num;
	uint32 size;
	uint32 addr;
	uint32 shndx;
} __attribute__((packed)) ELF_SECTION_HEADER_TABLE_T;

/* The Multiboot information. */
typedef struct MULTIBOOT_INFO
{
	uint32 flags;
	uint32 mem_lower;
	uint32 mem_upper;
	uint32 boot_device;
	uint32 cmdline;
	uint32 mods_count;
	uint32 mods_addr;
	union
	{
		AOUT_SYMBOL_TABLE_T aout_sym;
		ELF_SECTION_HEADER_TABLE_T elf_sec;
	} u;
	uint32 mmap_length;
	uint32 mmap_addr;
	uint32 drives_length;
	uint32 drives_addr;
	uint32 config_table;
	uint32 boot_loader_name;
	uint32 apm_table;
	uint32 vbe_control_info;
	uint32 vbe_mode_info;
	uint32 vbe_mode;
	uint32 vbe_interface_seg;
	uint32 vbe_interface_off;
	uint32 vbe_interface_len;
} __attribute__((packed)) MULTIBOOT_INFO_T;

/* The module structure. */
typedef struct MODULE
{
	uint32 mod_start;
	uint32 mod_end;
	uint32 string;
	uint32 reserved;
} __attribute__((packed)) MODULE_T;

/* The memory map. Be careful that the offset 0 is base_addr_low
but no size. */
typedef struct MEMORY_MAP
{
	uint32 size;
	uint32 base_addr_low;
	uint32 base_addr_high;
	uint32 length_low;
	uint32 length_high;
	uint32 type;
} __attribute__((packed)) MEMORY_MAP_T;
