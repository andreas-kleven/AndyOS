#include <Drawing/vbe.h>
#include <video.h>
#include <string.h>

VBEVideoMode::VBEVideoMode(VBE_MODE_INFO* info) 
	: VideoMode(info->width, info->height, info->bpp, info->framebuffer)
{
	this->info = *info;
}

void VBEVideoMode::Draw(void* pixels)
{
	memcpy(framebuffer, pixels, memsize);
}

void VBEVideoMode::SetPixel(int x, int y, unsigned int col)
{
	unsigned int* a = (unsigned int*)framebuffer + y * width + x;
	*a = col;
}