#pragma once
#include "definitions.h"

static class Disk
{
public:
	static STATUS Read(int sector, char*& buffer, int length);
};