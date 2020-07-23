#pragma once
#include "pmem.h"
#include "vmem.h"

#define BLOCK_SIZE			0x1000
#define MEMORY_MAP_SIZE		0x20000

#define KERNEL_BASE			0x1000000
#define KERNEL_END			USER_BASE
#define USER_BASE			0x40000000
#define HEAP_END			0x80000000
#define STACK_BASE			0xFE000000
#define STACK_END			USER_END
#define USER_END			0xFF000000

#define BYTES_TO_BLOCKS(x)	((1 + (((x) - 1) / BLOCK_SIZE)) * ((x) != 0))
