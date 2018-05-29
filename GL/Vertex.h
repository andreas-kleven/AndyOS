#pragma once
#include "Matrix4.h"
#include "Vector4.h"
#include "Color.h"

struct Vertex
{
public:
	Vector4 pos;
	Vector4 tmpPos;

	ColRGB color;
	ColRGB builtColor;

	Vector4 normal;
	Vector4 worldNormal;

	float tex_u;
	float tex_v;

	Vertex();
	Vertex(float x, float y, float z, ColRGB col, Vector4 normal);

	void MulMatrix(const Matrix4& mat);
};
