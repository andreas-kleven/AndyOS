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

    void SetFramebuffer(void *virt, void *phys)
    {
        this->framebuffer = virt;
        this->framebuffer_phys = phys;
    }

    void Draw(void *pixels);
    void SetPixel(int x, int y, unsigned int col);
    unsigned int GetPixel(int x, int y);
};

namespace Video {
extern VideoMode *mode;
void SetMode(VideoMode *mode);
} // namespace Video
