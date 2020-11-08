#pragma once
#include <andyos/image.h>
#include <sys/types.h>

struct BMP_HEADER
{
    uint16_t sig;
    uint32_t filesize;
    uint32_t reserved;
    uint32_t dataOffset;

    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compression;
} __attribute__((packed));

struct BMP : public IMAGE
{
    BMP_HEADER header;

    static BMP *Parse(void *buffer, size_t size);
};
