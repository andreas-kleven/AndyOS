#pragma once
#include <stdlib.h>

#define BLOCK_SIZE	0x1000
#define BYTES_TO_BLOCKS(x)	((1 + (((x) - 1) / BLOCK_SIZE)) * ((x) != 0))

inline void* operator new(size_t size)
{
	return malloc(size);
}

inline void operator delete(void* p)
{
    free(p);
}

inline void* operator new[](size_t size)
{
    return malloc(size);
}

inline void operator delete[](void* p)
{
    free(p);
}

inline void operator delete(void* p, size_t size)
{
    // TODO
    return free(p);
}
