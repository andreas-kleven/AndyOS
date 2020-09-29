#include "Rasterizer.h"
#include "GL.h"
#include <andyos/math.h>
#include <math.h>

float EdgeFunction(const Vector4 &a, const Vector4 &b, const Vector4 &c)
{
    return (c.y - a.y) * (b.x - a.x) - (c.x - a.x) * (b.y - a.y);
}

float EdgeFunction(const Vector4 &a, const Vector4 &b, float cx, float cy)
{
    return (cy - a.y) * (b.x - a.x) - (cx - a.x) * (b.y - a.y);
}

Rasterizer::Rasterizer()
{
    this->depth_buffer = 0;
}

Rasterizer::Rasterizer(GC gc)
{
    this->gc = gc;
    this->depth_buffer = new float[gc.width * gc.height];
}

void Rasterizer::Clear()
{
    memset(this->depth_buffer, 0, gc.width * gc.height * sizeof(float));
}

void Rasterizer::DrawTriangle(const Vertex &v0, const Vertex &v1, const Vertex &v2, BMP *texture)
{
    // gc.DrawLine((int)v0.tmp_pos.x, (int)v0.tmp_pos.y, (int)v1.tmp_pos.x, (int)v1.tmp_pos.y,
    // Color::Red); gc.DrawLine((int)v1.tmp_pos.x, (int)v1.tmp_pos.y, (int)v2.tmp_pos.x,
    // (int)v2.tmp_pos.y,
    //            Color::Green);
    // gc.DrawLine((int)v2.tmp_pos.x, (int)v2.tmp_pos.y, (int)v0.tmp_pos.x, (int)v0.tmp_pos.y,
    //            Color::Blue);

    // Clamping
    int minx = min(floor(v0.tmp_pos.x), floor(v1.tmp_pos.x), floor(v2.tmp_pos.x));
    int maxx = max(floor(v0.tmp_pos.x), floor(v1.tmp_pos.x), floor(v2.tmp_pos.x));
    int miny = min(floor(v0.tmp_pos.y), floor(v1.tmp_pos.y), floor(v2.tmp_pos.y));
    int maxy = max(floor(v0.tmp_pos.y), floor(v1.tmp_pos.y), floor(v2.tmp_pos.y));

    minx = clamp(minx, gc.x, (int)gc.width - 1);
    maxx = clamp(maxx, gc.x, (int)gc.width - 1);
    miny = clamp(miny, gc.y, (int)gc.height - 1);
    maxy = clamp(maxy, gc.y, (int)gc.height - 1);

    float area = EdgeFunction(v0.tmp_pos, v1.tmp_pos, v2.tmp_pos);
    float inv_area = 1 / area;

    int line_delta = gc.width - (maxx - minx) - 1;

    uint32_t *color_ptr = (uint32_t *)(gc.framebuffer + miny * gc.stride + minx);
    float *depth_ptr = (float *)(depth_buffer + miny * gc.width + minx);

    float co[3][3] = {{v0.built_color.r * v0.tmp_pos.w, v1.built_color.r * v1.tmp_pos.w,
                       v2.built_color.r * v2.tmp_pos.w},
                      {v0.built_color.g * v0.tmp_pos.w, v1.built_color.g * v1.tmp_pos.w,
                       v2.built_color.g * v2.tmp_pos.w},
                      {v0.built_color.b * v0.tmp_pos.w, v1.built_color.b * v1.tmp_pos.w,
                       v2.built_color.b * v2.tmp_pos.w}};

    float A01 = (v0.tmp_pos.y - v1.tmp_pos.y) * inv_area;
    float B01 = (v1.tmp_pos.x - v0.tmp_pos.x) * inv_area;
    float A12 = (v1.tmp_pos.y - v2.tmp_pos.y) * inv_area;
    float B12 = (v2.tmp_pos.x - v1.tmp_pos.x) * inv_area;
    float A20 = (v2.tmp_pos.y - v0.tmp_pos.y) * inv_area;
    float B20 = (v0.tmp_pos.x - v2.tmp_pos.x) * inv_area;

    float w0_row = EdgeFunction(v1.tmp_pos, v2.tmp_pos, minx, miny) * inv_area;
    float w1_row = EdgeFunction(v2.tmp_pos, v0.tmp_pos, minx, miny) * inv_area;
    float w2_row = EdgeFunction(v0.tmp_pos, v1.tmp_pos, minx, miny) * inv_area;

    // Drawing loop
    for (int y = miny; y <= maxy; y++) {
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;

        for (int x = minx; x <= maxx; x++) {
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float Z = 1 / (v0.tmp_pos.w * w0 + v1.tmp_pos.w * w1 + v2.tmp_pos.w * w2);

                if (*depth_ptr == 0 || Z < *depth_ptr) {
                    float U = ((w0 * v0.tmp_u * v0.tmp_pos.w) + (w1 * v1.tmp_u * v1.tmp_pos.w) +
                               (w2 * v2.tmp_u * v2.tmp_pos.w)) *
                              Z;
                    float V = ((w0 * v0.tmp_v * v0.tmp_pos.w) + (w1 * v1.tmp_v * v1.tmp_pos.w) +
                               (w2 * v2.tmp_v * v2.tmp_pos.w)) *
                              Z;

                    float r = (co[0][0] * w0 + co[0][1] * w1 + co[0][2] * w2);
                    float g = (co[1][0] * w0 + co[1][1] * w1 + co[1][2] * w2);
                    float b = (co[2][0] * w0 + co[2][1] * w1 + co[2][2] * w2);

                    r *= Z;
                    g *= Z;
                    b *= Z;

                    float lum = 0.21 * r + 0.72 * g + 0.07 * b;

                    if (texture) {
                        int X = texture->width * U;
                        int Y = texture->height * V;

                        uint32_t color = texture->pixels[(int)(Y * texture->width + X)];
                        r = lum * (uint8_t)(color >> 16);
                        g = lum * (uint8_t)(color >> 8);
                        b = lum * (uint8_t)(color >> 0);
                    } else {
                        r *= 255;
                        g *= 255;
                        b *= 255;
                    }

                    int col = ((int)(r) << 16) | ((int)(g) << 8) | (int)(b);

                    *color_ptr = col;
                    *depth_ptr = Z;
                }
            }

            w0 += A12;
            w1 += A20;
            w2 += A01;

            color_ptr++;
            depth_ptr++;
        }

        w0_row += B12;
        w1_row += B20;
        w2_row += B01;

        color_ptr += line_delta;
        depth_ptr += line_delta;
    }
}
