#pragma once
#include "definitions.h"
#include "Vertex.h"

namespace gl
{
	static class Rasterizer
	{
	public:
		static uint32* color_buffer;
		static float* depth_buffer;

		static STATUS Init();

		static void DrawTriangle(Vertex* buffer, BMP* texture);
	};
}