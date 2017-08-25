#pragma once
#include "Matrix.h"
#include "Color.h"

namespace gl
{
	struct Vertex
	{
	public:
		float x;
		float y;
		float z;

		float mul_x;
		float mul_y;
		float mul_z;
		float mul_w;

		ColRGB color;
		Vector4 normal;

		ColRGB built_color;

		float tex_u;
		float tex_v;

		Vertex();
		Vertex(float x, float y, float z, ColRGB col, Vector4 normal);

		void MulMatrix(const Matrix& mat);
	};
}