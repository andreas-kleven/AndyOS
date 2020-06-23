#pragma once
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

struct BMP
{
	BMP_HEADER header;

	uint32_t* pixels;
	uint32_t pixel_count;
	uint32_t size;

	int width;
	int height;

	BMP(void* buffer);
	BMP(void* pixels, int width, int height);
};