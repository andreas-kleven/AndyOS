#include "bmp.h"
#include "string.h"
#include "debug.h"
#include "drawing.h"

BMP::BMP(void* buffer)
{
	BMP_HEADER* bmp = (BMP_HEADER*)buffer;

	this->width = bmp->width;
	this->height = bmp->height;
	this->pixel_count = width * height;
	this->size = pixel_count * 4;

	int bytes = bmp->bpp / 8;
	int pitch = bmp->width * bytes;

	uint8* p = (uint8*)bmp + bmp->dataOffset;

	//Debug::Print("%i\n", bytes);
	//Debug::Print("%i\n", pitch);
	//Debug::Print("%i\n", width);
	//Debug::Print("%i\n", height);
	//while (1);

	pixels = new uint32[size];
	uint32* p_ptr = pixels;

	int count = pixel_count;

	while (count--)
	{
		*p_ptr++ = *(uint32*)p;
		p += bytes;
	}
	return;

	for (int _y = 0; _y < height; _y++)
	{
		uint8* _p = p;

		for (int _x = 0; _x < width; _x++)
		{
			//VBE::SetPixel(_x, _y, *(uint32*)p);
			*p_ptr++ = *(uint32*)p;
			p += bytes;
		}

		p = _p + pitch;
	}

	Drawing::Draw();
}

BMP::BMP(void* buffer, int width, int height)
{
	this->width = width;
	this->height = height;
	this->size = width * height * 4;
	pixels = new uint32[size];
	memcpy(pixels, buffer, size);
}
