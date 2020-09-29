#include "Vertex.h"

Vertex::Vertex()
{
    this->u = 0;
    this->v = 0;
}

Vertex::Vertex(float x, float y, float z, Color col, Vector4 normal) : Vertex()
{
    this->pos.x = x;
    this->pos.y = y;
    this->pos.z = z;
    this->pos.w = 1;

    this->color = col;
    this->built_color = col;

    this->normal = normal;
    this->world_normal = normal;
}

void Vertex::MulMatrix(const Matrix4 &mat, bool reset)
{
    if (reset)
        tmp_pos = mat * pos;
    else
        tmp_pos = mat * tmp_pos;
}
