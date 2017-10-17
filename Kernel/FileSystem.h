#pragma once
#include "definitions.h"

class FileSystem
{
public:
	static STATUS ReadFile(char* name, char*& buffer);
};