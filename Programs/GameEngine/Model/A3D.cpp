#include "A3D.h"
#include <stdio.h>

struct RAW_VERTEX
{
    float pos_x;
    float pos_y;
    float pos_z;

    float norm_x;
    float norm_y;
    float norm_z;

    float tex_u;
    float tex_v;
};

A3D::A3D(char *buffer)
{
    int vertex_count = *(uint32_t *)buffer;
    vertex_buffer = new Vertex[vertex_count];
    triangle_buffer = new Triangle[vertex_count / 3];

    RAW_VERTEX *vertex = (RAW_VERTEX *)((char *)buffer + 4);

    for (int i = 0; i < vertex_count; i++) {
        RAW_VERTEX raw = *vertex++;

        Color col(1, 1, 1);
        Vector4 norm = Vector4(raw.norm_x, raw.norm_y, raw.norm_z, 1);
        Vertex vert(raw.pos_x, raw.pos_y, raw.pos_z, col, norm);
        vert.tex_u = raw.tex_u;
        vert.tex_v = raw.tex_v;

        vertex_buffer[i] = vert;
        vertices.push_back(&vertex_buffer[i]);
    }

    for (int i = 0; i < vertex_count; i += 3) {
        Triangle tri(&vertex_buffer[i], &vertex_buffer[i + 1], &vertex_buffer[i + 2]);
        triangle_buffer[i / 3] = tri;
        triangles.push_back(&triangle_buffer[i / 3]);
    }
}