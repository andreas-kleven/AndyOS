#pragma once
#include "IMesh.h"
#include "Transform.h"

#include "../GL/GL.h"

class IMesh
{
public:
	gl::Vertex* vertices;
	int vertex_count;
};