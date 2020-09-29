#pragma once
#include "Matrix4.h"
#include "Vector4.h"
#include <andyos/drawing.h>

struct Vertex
{
    Vector4 pos;
    Vector4 tmp_pos;

    Color color;
    Color built_color;

    Vector4 normal;
    Vector4 world_normal;

    float u;
    float v;

    float tmp_u;
    float tmp_v;

    Vertex();
    Vertex(float x, float y, float z, Color col, Vector4 normal);

    void MulMatrix(const Matrix4 &mat, bool reset);
};
