#include <memory.h>
#include <string.h>
#include <video.h>

void VideoMode::Draw(void *pixels)
{
    if (framebuffer == 0 || pixels == 0)
        return;

    memcpy(framebuffer, pixels, memsize);
}

void VideoMode::SetPixel(int x, int y, unsigned int col)
{
    if (framebuffer == 0)
        return;

    if (x >= width || x < 0)
        return;

    if (y >= height || y < 0)
        return;

    unsigned int *a = (unsigned int *)framebuffer + y * width + x;
    *a = col;
}

unsigned int VideoMode::GetPixel(int x, int y)
{
    if (framebuffer == 0)
        return 0;

    if (x >= width || x < 0)
        return 0;

    if (y >= height || y < 0)
        return 0;

    unsigned int *a = (unsigned int *)framebuffer + y * width + x;
    return *a;
}

namespace Video {
VideoMode *mode;

void SetMode(VideoMode *_mode)
{
    mode = _mode;

    if (mode->framebuffer_phys != 0 && mode->framebuffer == 0) {
        // Map framebuffer
        mode->framebuffer = VMem::KernelMapFirstFree(
            mode->framebuffer_phys, BYTES_TO_BLOCKS(mode->memsize), PAGE_PRESENT | PAGE_WRITE);
    }
}
} // namespace Video
