#pragma once
#include "Vertex.h"
#include <AndyOS.h>
#include <sys/types.h>

class Rasterizer
{
  private:
    GC gc;
    float *depth_buffer;

  public:
    Rasterizer();
    Rasterizer(GC gc);

    void Clear();
    void DrawTriangle(const Vertex &v0, const Vertex &v1, const Vertex &v2, BMP *texture);
};
