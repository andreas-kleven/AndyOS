#pragma once
#include "Matrix4.h"
#include "Vector4.h"
#include <andyos/drawing.h>

struct Vertex
{
    Vector4 pos;
    Vector4 tmpPos;

    Color color;
    Color builtColor;

    Vector4 normal;
    Vector4 worldNormal;

    float tex_u;
    float tex_v;

    Vertex();
    Vertex(float x, float y, float z, Color col, Vector4 normal);

    void MulMatrix(const Matrix4 &mat);
};
