#include "Vertex.h"

Vertex::Vertex()
{
	this->tex_u = 0;
	this->tex_v = 0;
}

Vertex::Vertex(float x, float y, float z, Color col, Vector4 normal) : Vertex()
{
	this->pos.x = x;
	this->pos.y = y;
	this->pos.z = z;
	this->pos.w = 1;

	this->color = col;
	this->builtColor = col;

	this->normal = normal;
	this->worldNormal = normal;
}

void Vertex::MulMatrix(const Matrix4& mat)
{
	tmpPos = mat * pos;
}