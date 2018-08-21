#pragma once
#include "types.h"
#include "Drivers/driver.h"

class MouseDriver : public CharDriver
{
public:
	MouseDriver();

	int Open(FNODE* node, FILE* file);
	int Read(FILE* file, char* buf, size_t size);
};