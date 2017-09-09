#pragma once
#include "Matrix4.h"
#include "Color.h"

namespace gl
{
	struct Vertex
	{
	public:
		Vector4 pos;
		Vector4 mul_pos;

		ColRGB color;
		Vector4 normal;

		ColRGB built_color;

		float tex_u;
		float tex_v;

		Vertex();
		Vertex(float x, float y, float z, ColRGB col, Vector4 normal);

		void MulMatrix(const Matrix4& mat);
	};
}