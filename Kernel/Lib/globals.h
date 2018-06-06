#pragma once
#include "Memory/memory.h"
#include "math.h"
#include "string.h"
#include "debug.h"

struct ARRAY_ALLOC
{
	uint32 blocks;
	uint32 addr;
};

int mem_left = 0;
uint8* mem_ptr = 0;

void* operator new(unsigned size)
{
	if (!size)
		return 0;

	/*if (size > mem_left)
	{
		int blocks = ceil(size / 4096.f);
		uint8* ptr = (uint8*)Memory::AllocBlocks(blocks);
		mem_ptr = ptr + size;
		mem_left = size % BLOCK_SIZE;
		return ptr;
	}
	else
	{
		uint8* ptr = mem_ptr;
		mem_ptr += size;
		mem_left -= size;
		return ptr;
	}*/

	int blocks = BYTES_TO_BLOCKS(size);
	void* addr = VMem::KernelAlloc(VMem::GetCurrentDir(), blocks);
	return addr;
}
void operator delete(void* p)
{
	//Memory::FreeBlocks(p, 1);
}

void* operator new[](unsigned size)
{
	if (size == 0)
		return 0;

	if (size > mem_left)
	{
		int blocks = BYTES_TO_BLOCKS(size);

		uint8* ptr = (uint8*)VMem::KernelAlloc(VMem::GetCurrentDir(), blocks);
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
	//ARRAY_ALLOC* header = (ARRAY_ALLOC*)((uint8*)p - BLOCK_SIZE);

	//if (blocks != 0)
	//Debug::Print("%ux ", (char)header->blocks);

	//Debug::Print("\n\n");
	//Debug::Dump(header, 100);
	//
	//if (header->blocks > 10)
	//{
	//	while (1);
	//}

	//Memory::FreeBlocks(header, header->blocks);
}