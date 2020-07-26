#pragma once
#include <types.h>

struct GDT_PTR
{
	uint16 limit;
	uint32 base;
} __attribute__((packed));

namespace GDT
{
	STATUS SetDescriptor(uint32 i, uint32 base, uint32 limit, uint32 flag);
	STATUS Init();
};

