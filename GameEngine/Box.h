#pragma once
#include "Vector.h"

struct Box
{
	Vector min;
	Vector max;

	Box() { }

	Box(Vector min, Vector max)
	{
		this->min = min;
		this->max = max;
	}

	bool IsInside(const Vector& in)
	{
		return ((in.x > min.x) && (in.x < max.x) && (in.y > min.y) && (in.y < max.y) && (in.z > min.z) && (in.z < max.z));
	}

	bool IsInside(const Box& other)
	{
		return (IsInside(other.min) && IsInside(other.max));
	}
};