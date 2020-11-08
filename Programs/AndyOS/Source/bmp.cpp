#include <AndyOS.h>
#include <andyos/bmp.h>
#include <string.h>
#include <sys/types.h>

BMP *BMP::Parse(void *buffer, size_t size)
{
    if (size < sizeof(BMP_HEADER))
        return 0;

    BMP_HEADER *header = (BMP_HEADER *)buffer;

    int bytes = header->bpp / 8;
    int pitch = header->width * bytes;

    BMP *bmp = new BMP();
    bmp->width = header->width;
    bmp->height = header->height;
    size_t pixel_count = bmp->width * bmp->height;
    size_t data_size = pixel_count * bytes;

    uint8_t *p = (uint8_t *)buffer + header->dataOffset + data_size - bytes;

    bmp->pixels = new uint32_t[pixel_count];
    uint32_t *p_ptr = bmp->pixels;

    int count = pixel_count;

    while (count--) {
        *p_ptr++ = *(uint32_t *)p;
        p -= bytes;
    }

    bmp->header = *header;
    return bmp;
}
