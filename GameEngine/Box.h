#pragma once
#include "../GL/Vector3.h"

struct Box
{
	Vector3 min;
	Vector3 max;

	Box() { }

	Box(Vector3 min, Vector3 max)
	{
		this->min = min;
		this->max = max;
	}

	bool IsInside(const Vector3& in)
	{
		return ((in.x > min.x) && (in.x < max.x) && (in.y > min.y) && (in.y < max.y) && (in.z > min.z) && (in.z < max.z));
	}

	bool IsInside(const Box& other)
	{
		return (IsInside(other.min) && IsInside(other.max));
	}
};