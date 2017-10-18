#pragma once
#include "definitions.h"

class FS
{
public:
	static STATUS Init();
	static STATUS ReadFile(char* name, char*& buffer);
};