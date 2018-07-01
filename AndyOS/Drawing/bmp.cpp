#include "bmp.h"
#include "string.h"
#include "drawing.h"
#include <AndyOS.h>

BMP::BMP(void* buffer)
{
	header = *(BMP_HEADER*)buffer;

	this->width = header.width;
	this->height = header.height;
	this->pixel_count = width * height;
	this->size = pixel_count * 4;

	int bytes = header.bpp / 8;
	int pitch = header.width * bytes;

	uint8* p = (uint8*)buffer + header.dataOffset + size - bytes;

	pixels = new uint32[pixel_count];
	uint32* p_ptr = pixels;

	int count = pixel_count;

	while (count--)
	{
		*p_ptr++ = *(uint32*)p;
		p -= bytes;
	}
}

BMP::BMP(void* pixels, int width, int height)
{
	this->width = width;
	this->height = height;
	this->size = width * height * 4;
	pixels = new uint32[size];
	memcpy(this->pixels, pixels, size);
}
