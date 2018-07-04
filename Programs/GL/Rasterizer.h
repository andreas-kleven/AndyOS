#pragma once
#include <AndyOS.h>
#include "definitions.h"
#include "Vertex.h"

class Rasterizer
{
private:
	GC gc;
	float* depth_buffer;

public:
	Rasterizer();
	Rasterizer(GC gc);
	
	void Clear();
	void DrawTriangle(Vertex& v0, Vertex& v1, Vertex& v2, BMP* texture);
	void DrawTriangle2(Vertex& v0, Vertex& v1, Vertex& v2, BMP* texture);
};