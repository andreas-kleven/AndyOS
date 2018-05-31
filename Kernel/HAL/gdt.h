#pragma once
#include "definitions.h"

struct GDT_PTR
{
	uint16 limit;
	uint32 base;
};

static class GDT
{
public:
	static STATUS Init();
	static STATUS GDT::SetDescriptor(uint32 i, uint32 base, uint32 limit, uint32 flag);
};

