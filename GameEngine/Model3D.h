#pragma once
#include "../GL/GL.h"
#include "Vector3.h"

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
	gl::Vertex* vertices;

	//void SetTexture(int index);
};