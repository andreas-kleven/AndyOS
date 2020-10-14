#pragma once
#include <Memory/pmem.h>
#include <Memory/vmem.h>

#define BLOCK_SIZE        0x1000
#define MEMORY_MAP_SIZE   0x20000
#define MAX_BLOCKS        0x100000
#define KERNEL_STACK_SIZE 0x10000
#define VMEM_TABLES_SIZE  BLOCK_SIZE * 1025

#define KERNEL_PHYS  0x1000000
#define HEAP_END     0x80000000
#define USER_BASE    0x10000000
#define STACK_END    USER_END
#define USER_END     KERNEL_BASE
#define STACK_BASE   0xB0000000
#define KERNEL_BASE  0xC0000000
#define KERNEL_END   0xFF000000
#define KERNEL_STACK 0xFFB00000
#define MEMORY_END   0xFFFFF000

#define BYTES_TO_BLOCKS(x) ((1 + (((x)-1) / BLOCK_SIZE)) * ((x) != 0))
