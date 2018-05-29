#pragma once
#include "../GL/Vector3.h"

struct Box
{
	Vector3 min;
	Vector3 max;

	Box()
	{
		this->min = Vector3(0, 0, 0);
		this->max = Vector3(0, 0, 0);
	}

	Box(Vector3& min, Vector3& max)
	{
		this->min = min;
		this->max = max;
	}

	void Translate(Vector3& translation)
	{
		min += translation;
		max += translation;
	}

	void Scale(Vector3& scaling)
	{
		min = Vector3(min.x * scaling.x, min.y * scaling.y, min.z * scaling.z);
		max = Vector3(max.x * scaling.x, max.y * scaling.y, max.z * scaling.z);
	}

	void Expand(Box box)
	{
		min.x = ::min(min.x, box.min.x);
		min.y = ::min(min.y, box.min.y);
		min.z = ::min(min.z, box.min.z);

		max.x = ::max(max.x, box.max.x);
		max.y = ::max(max.y, box.max.y);
		max.z = ::max(max.z, box.max.z);
	}

	Vector3 Size()
	{
		return max - min;
	}

	int LongestAxis()
	{
		Vector3& size = max - min;

		if (size.x > size.z)
		{
			return size.x < size.y;
		}
		else
		{
			return (size.y > size.z) ? 1 : 2;
		}
	}

	bool IsInside(const Vector3& in)
	{
		return ((in.x >= min.x) && (in.x <= max.x) && (in.y >= min.y) && (in.y <= max.y) && (in.z >= min.z) && (in.z <= max.z));
	}

	bool IsInside(const Box& other)
	{
		return (IsInside(other.min) && IsInside(other.max));
	}

	bool Overlaps(const Box& other)
	{
		return (IsInside(other.min) || IsInside(other.max));
	}

	//https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
	bool RayIntersection(Vector3& rayOrigin, Vector3& rayDir, float& t)
	{
		Vector3 dirfrac;
		dirfrac.x = 1.0f / rayDir.x;
		dirfrac.y = 1.0f / rayDir.y;
		dirfrac.z = 1.0f / rayDir.z;

		float t1 = (min.x - rayOrigin.x) * dirfrac.x;
		float t2 = (max.x - rayOrigin.x) * dirfrac.x;
		float t3 = (min.y - rayOrigin.y) * dirfrac.y;
		float t4 = (max.y - rayOrigin.y) * dirfrac.y;
		float t5 = (min.z - rayOrigin.z) * dirfrac.z;
		float t6 = (max.z - rayOrigin.z) * dirfrac.z;

		float tmin = ::max(::max(::min(t1, t2), ::min(t3, t4)), ::min(t5, t6));
		float tmax = ::min(::min(::max(t1, t2), ::max(t3, t4)), ::max(t5, t6));

		// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
		if (tmax < 0)
		{
			t = tmax;
			return false;
		}

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax)
		{
			t = tmax;
			return false;
		}

		t = tmin;
		return true;
	}
};