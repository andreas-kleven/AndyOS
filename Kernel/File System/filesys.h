#pragma once
#include "definitions.h"

class FS
{
public:
	static STATUS Init();
	static uint32 ReadFile(char* name, char*& buffer);
};