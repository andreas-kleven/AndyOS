#include <Drawing/vbe.h>
#include <string.h>
#include <video.h>

VBEVideoMode::VBEVideoMode(VBE_MODE_INFO *info)
    : VideoMode(info->width, info->height, info->bpp, info->framebuffer)
{
    this->info = *info;
}
