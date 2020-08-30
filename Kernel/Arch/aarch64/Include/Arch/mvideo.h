#pragma once
#include <video.h>

class MVideoMode : public VideoMode
{
  public:
    MVideoMode(int width, int height, int depth);

    void Draw(void *pixels);
    void SetPixel(int x, int y, unsigned int col);
};