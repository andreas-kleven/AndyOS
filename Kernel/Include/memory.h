#pragma once
#include <Memory/pmem.h>
#include <Memory/vmem.h>

#define BLOCK_SIZE			0x1000
#define MEMORY_MAP_SIZE		0x20000
#define MAX_BLOCKS          0x100000

#define KERNEL_BASE			0x1000000
#define KERNEL_END			USER_BASE
#define USER_BASE			0x40000000
#define HEAP_END			0x80000000
#define STACK_BASE			0xE0000000
#define STACK_END			USER_END
#define USER_END			0xF0000000
#define MEMORY_END          0xFFFFF000

#define BYTES_TO_BLOCKS(x)	((1 + (((x) - 1) / BLOCK_SIZE)) * ((x) != 0))
