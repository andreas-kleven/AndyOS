#pragma once
#include <driver.h>
#include <keycodes.h>
#include <types.h>

class KeyboardDriver : public CharDriver
{
  public:
    KeyboardDriver();

    int Open(FILE *file);
    int Read(FILE *file, void *buf, size_t size);
};
