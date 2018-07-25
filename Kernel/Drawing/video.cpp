#include "video.h"
#include "Memory/memory.h"
#include "string.h"

namespace Video
{
    VideoMode* mode;

    void SetMode(VideoMode* _mode)
    {
        mode = _mode;

        if (mode->framebuffer_phys != 0 && mode->framebuffer == 0)
        {
            //Map framebuffer
            mode->framebuffer = VMem::KernelMapFirstFree(
                mode->framebuffer_phys, 
                BYTES_TO_BLOCKS(mode->memsize), 
                PAGE_PRESENT | PAGE_WRITE);
        }
    }

    void Draw(void* pixels)
    {
        if (mode->framebuffer == 0 || pixels == 0)
            return;

        mode->Draw(pixels);
    }

	void SetPixel(int x, int y, unsigned int col)
    {
        if (mode->framebuffer == 0)
            return;

        if (x >= mode->width || x < 0)
			return;

		if (y >= mode->height || y < 0)
			return;

		mode->SetPixel(x, y, col);
    }
}