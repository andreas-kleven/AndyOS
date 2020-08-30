#include <AndyOS.h>
#include <andyos/bmp.h>
#include <string.h>
#include <sys/types.h>

BMP::BMP(void *buffer)
{
    header = *(BMP_HEADER *)buffer;

    int bytes = header.bpp / 8;
    int pitch = header.width * bytes;

    this->width = header.width;
    this->height = header.height;
    this->pixel_count = width * height;
    this->size = pixel_count * bytes;

    uint8_t *p = (uint8_t *)buffer + header.dataOffset + size - bytes;

    pixels = new uint32_t[pixel_count];
    uint32_t *p_ptr = pixels;

    int count = pixel_count;

    while (count--) {
        *p_ptr++ = *(uint32_t *)p;
        p -= bytes;
    }
}

BMP::BMP(void *pixels, int width, int height)
{
    this->width = width;
    this->height = height;
    this->size = width * height * 4;
    pixels = new uint32_t[size];
    memcpy(this->pixels, pixels, size);
}
