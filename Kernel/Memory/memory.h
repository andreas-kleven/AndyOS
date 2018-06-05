#pragma once
#include "pmem.h"
#include "vmem.h"

#define MEMORY_SIZE			0xFFFFFFFF //4 GB
#define BLOCK_SIZE			0x1000
#define BLOCK_COUNT			(MEMORY_SIZE / BLOCK_SIZE)
#define MEMORY_MAP_SIZE		0x20000
#define KERNEL_SIZE			0x20000000 //1/2 GB
#define KERNEL_BASE_PHYS	0x100000

#define KERNEL_BASE			0x100000
#define KERNEL_END			USER_BASE
#define USER_BASE			0x40000000
#define USER_END			MEMORY_SIZE

#define BYTES_TO_BLOCKS(x)	(1 + (((x) - 1) / BLOCK_SIZE))