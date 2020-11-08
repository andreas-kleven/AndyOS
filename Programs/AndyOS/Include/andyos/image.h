#pragma once
#include <stdint.h>

struct IMAGE
{
    uint32_t *pixels;
    int width;
    int height;

    static IMAGE *Load(const char *filename);
};
