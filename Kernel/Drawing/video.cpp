#include "video.h"
#include "string.h"

namespace Video
{
    VIDEO_MODE mode;
    int memsize;

    void SetMode(VIDEO_MODE _mode)
    {
        mode = _mode;
        memsize = mode.width * mode.height * mode.depth / 8;
    }

    void Draw(void* pixels)
    {
        if (mode.framebuffer == 0)
            return;

        memcpy(mode.framebuffer, pixels, memsize);
    }

	void SetPixel(int x, int y, unsigned int col)
    {
        if (mode.framebuffer == 0)
            return;

        if (x >= mode.width || x < 0)
			return;

		if (y >= mode.height || y < 0)
			return;

		unsigned int* a = (unsigned int*)mode.framebuffer + y * mode.width + x;
		*a = col;
    }
}