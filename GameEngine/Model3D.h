#pragma once
#include "List.h"
#include "Box.h"
#include "../GL/GL.h"
#include "../GL/Vertex.h"

struct Triangle
{
	Vertex* v0;
	Vertex* v1;
	Vertex* v2;

	Triangle()
	{
		v0 = 0;
		v1 = 0;
		v2 = 0;
	}

	Triangle(Vertex* _v0, Vertex* _v1, Vertex* _v2)
	{
		v0 = _v0;
		v1 = _v1;
		v2 = _v2;
	}

	Vector3 Normal(float u, float v)
	{
		return ((v0->normal * (1 - u - v)) + (v1->normal * u) + (v2->normal * v)).ToVector3();
	}

	Vector3 WorldNormal(float u, float v)
	{
		return ((v0->worldNormal * (1 - u - v)) + (v1->worldNormal * u) + (v2->worldNormal * v)).ToVector3();
	}

	Vector3 Center()
	{
		return (v0->pos + v1->pos + v2->pos).ToVector3() / 3;
	}

	ColRGB Color(float u, float v)
	{
		float w = (1 - u - v);

		ColRGB color;
		color.r = ((v0->color.r * w) + (v1->color.r * u) + (v2->color.r * v));
		color.g = ((v0->color.g * w) + (v1->color.g * u) + (v2->color.g * v));
		color.b = ((v0->color.b * w) + (v1->color.b * u) + (v2->color.b * v));

		return color;
	}

	Box BoundingBox()
	{
		Box bounds;
		bounds.min.x = min(v0->pos.x, v1->pos.x, v2->pos.x);
		bounds.min.y = min(v0->pos.y, v1->pos.y, v2->pos.y);
		bounds.min.z = min(v0->pos.z, v1->pos.z, v2->pos.z);

		bounds.max.x = max(v0->pos.x, v1->pos.x, v2->pos.x);
		bounds.max.y = max(v0->pos.y, v1->pos.y, v2->pos.y);
		bounds.max.z = max(v0->pos.z, v1->pos.z, v2->pos.z);
		return bounds;
	}

	Vertex* Get(int index)
	{
		if (index == 0)
			return v0;

		if (index == 1)
			return v1;

		if (index == 2)
			return v2;

		Exceptions::ThrowException("Index out of range exception", "Vertex index must be in the range 0-2");
	}

	Vertex* operator[](int index)
	{
		return Get(index);
	}
};

class Model3D
{
public:
	Vertex* vertex_buffer;
	Triangle* triangle_buffer;

	List<Vertex*> vertices;
	List<Triangle*> triangles;

	//void SetTexture(int index);
};