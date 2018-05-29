#pragma once
#include "math.h"

struct Vector3;

struct Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	Vector4();
	Vector4(float x, float y, float z, float w);

	Vector3 ToVector3();

	Vector4 operator+(Vector4& vec)
	{
		return Vector4(x + vec.x, y + vec.y, z + vec.z, w);
	}

	Vector4 operator-(Vector4& vec)
	{
		return Vector4(x - vec.x, y - vec.y, z - vec.z, w);
	}

	Vector4& operator-()
	{
		return Vector4(-x, -y, -z, w);
	}

	Vector4& operator*(float f)
	{
		return Vector4(x * f, y * f, z * f, w);
	}

	Vector4& operator/(float f)
	{
		float inv = 1 / f;
		return Vector4(x * inv, y * inv, z * inv, w);
	}

	float Magnitude()
	{
		return sqrt(x * x + y * y + z * z);
	}

	Vector4& Normalized()
	{
		return *this / Magnitude();
	}

	float Dot(Vector4& vec)
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	Vector4& Cross(const Vector4& vec)
	{
		return Vector4(y * vec.z - z * vec.y,
			z * vec.x - x * vec.z,
			x * vec.y - y * vec.x, w);
	}

	static float Dot(const Vector4& a, const Vector4& b);
	static Vector4 Cross(const Vector4& a, const Vector4& b);
};