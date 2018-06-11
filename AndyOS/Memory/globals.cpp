#pragma once
#include "Memory/memory.h"
#include "../API/api.h"
#include "globals.h"

void* operator new(unsigned size)
{
	if (!size)
		return 0;

	return alloc(BYTES_TO_BLOCKS(size));
}
void operator delete(void* p)
{

}

void operator delete(void * p, unsigned size)
{

}

void* operator new[](unsigned size)
{
	return alloc(BYTES_TO_BLOCKS(size));
}

void operator delete[](void* p)
{

}