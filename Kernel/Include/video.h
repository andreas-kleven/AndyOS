#pragma once

class VideoMode
{
  public:
    int width;
    int height;
    int depth;
    int memsize;

    void *framebuffer_phys = 0;
    void *framebuffer = 0;

    VideoMode() {}

    VideoMode(int width, int height, int depth, void *framebuffer)
    {
        this->width = width;
        this->height = height;
        this->depth = depth;
        this->framebuffer_phys = framebuffer;

        this->memsize = width * height * depth / 8;
    }

    virtual void Draw(void *pixels) {}
    virtual void SetPixel(int x, int y, unsigned int col) {}
};

namespace Video {
extern VideoMode *mode;

void SetMode(VideoMode *mode);
void Draw(void *pixels);
void SetPixel(int x, int y, unsigned int col);
} // namespace Video