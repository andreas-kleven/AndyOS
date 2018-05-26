#pragma once
#include "../GL/GL.h"

struct Face
{
	int positions[3];
	int normals[3];
	int uvs[3];
};

class Model3D
{
public:
	int vertex_count;
	Vertex* vertices;

	//void SetTexture(int index);
};