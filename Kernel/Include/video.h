#pragma once

struct VIDEO_MODE
{
    void* framebuffer = 0;
    int width;
    int height;
    int depth;

    VIDEO_MODE()
    { }

    VIDEO_MODE(void* framebuffer, int width, int height, int depth)
    {
        this->framebuffer = framebuffer;
        this->width = width;
        this->height = height;
        this->depth = depth;
    }
};

namespace Video
{
    extern VIDEO_MODE mode;

    void SetMode(VIDEO_MODE mode);
    void Draw(void* pixels);
	void SetPixel(int x, int y, unsigned int col);
}