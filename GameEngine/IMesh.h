#pragma once
#include "../GL/GL.h"
#include "IMesh.h"
#include "Transform.h"

class IMesh
{
public:
	gl::Vertex* vertices;
	int vertex_count;
};