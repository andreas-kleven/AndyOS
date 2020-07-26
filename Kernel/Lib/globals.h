#pragma once
#include <memory.h>
#include <math.h>
#include <types.h>
#include <string.h>
#include <debug.h>

size_t mem_left = 0;
uint8* mem_ptr = 0;

void* operator new(size_t size)
{
	if (!size)
		return 0;

	int blocks = BYTES_TO_BLOCKS(size);
	void* addr = VMem::KernelAlloc(blocks);
	return addr;
}
void operator delete(void* p)
{
	
}

void* operator new[](size_t size)
{
	int blocks = BYTES_TO_BLOCKS(size);
	void* addr = VMem::KernelAlloc(blocks);
	return addr;

	if (size == 0)
		return 0;

	if (size > mem_left)
	{
		int blocks = BYTES_TO_BLOCKS(size);

		uint8* ptr = (uint8*)VMem::KernelAlloc(blocks);
		mem_ptr = ptr + size;

		if (size < BLOCK_SIZE)
			mem_left = BLOCK_SIZE - (size % BLOCK_SIZE);
		else
			mem_left = 0;
		return ptr;
	}
	else
	{
		uint8* ptr = mem_ptr;
		mem_ptr += size;
		mem_left -= size;
		return ptr;
	}
}

void operator delete[](void* p)
{

}

void operator delete(void* p, size_t size)
{

}