#pragma once
#include "definitions.h"
#include "Vertex.h"
#include "Drawing\bmp.h"

namespace gl
{
	static class Rasterizer
	{
	public:
		static uint32* color_buffer;
		static float* depth_buffer;

		static STATUS Init();

		static void DrawTriangle(Vertex& v0, Vertex& v1, Vertex& v2, BMP* texture);
		static void DrawTriangle2(Vertex& v0, Vertex& v1, Vertex& v2, BMP* texture);
	};
}