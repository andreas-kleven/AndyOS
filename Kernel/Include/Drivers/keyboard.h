#pragma once
#include <types.h>
#include <driver.h>
#include <keycodes.h>

class KeyboardDriver : public CharDriver
{
public:
	KeyboardDriver();

	int Open(FILE *file);
	int Read(FILE *file, void *buf, size_t size);
};
