#pragma once
#include "definitions.h"

struct BMP_HEADER
{
	uint16 sig;
	uint32 filesize;
	uint32 reserved;
	uint32 dataOffset;

	uint32 size;
	uint32 width;
	uint32 height;
	uint16 planes;
	uint16 bpp;
	uint32 compression;
} __attribute__((packed));

struct BMP
{
	BMP_HEADER header;

	uint32* pixels;
	uint32 pixel_count;
	uint32 size;

	int width;
	int height;

	BMP(void* buffer);
	BMP(void* pixels, int width, int height);
};