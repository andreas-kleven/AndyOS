#include "API/api.h"
#include "globals.h"

void* operator new(long unsigned size)
{
	if (!size)
		return 0;

	return alloc(BYTES_TO_BLOCKS(size));
}

void operator delete(void* p)
{

}

void operator delete(void * p, long unsigned size)
{

}

void* operator new[](long unsigned size)
{
	if (!size)
		return 0;

	return alloc(BYTES_TO_BLOCKS(size));
}

void operator delete[](void* p)
{

}