#pragma once
#include "definitions.h"

class VFS
{
public:
	static STATUS Init();
	static uint32 ReadFile(char* path, char*& buffer);
};