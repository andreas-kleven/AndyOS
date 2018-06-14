#pragma once
#include "definitions.h"

struct GDT_PTR
{
	uint16 limit;
	uint32 base;
} __attribute__((packed));

class GDT
{
public:
	static STATUS Init();
	static STATUS SetDescriptor(uint32 i, uint32 base, uint32 limit, uint32 flag);
};

