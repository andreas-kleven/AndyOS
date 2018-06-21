#pragma once
#include "math.h"

struct Vector4;

struct Vector3
{
public:
	static Vector3 right;
	static Vector3 up;
	static Vector3 forward;

	float x = 0;
	float y = 0;
	float z = 0;

	Vector3();
	Vector3(float x, float y, float z);

	Vector3 EulerAngles() const;
	Vector4 ToVector4(float w = 1.0f) const;

	bool operator==(const Vector3& b)
	{
		return x == b.x && y == b.y && z == b.z;
	}

	Vector3 operator+(const Vector3& b) const
	{
		return Vector3(x + b.x, y + b.y, z + b.z);
	}

	Vector3& operator+=(const Vector3& b)
	{
		this->x += b.x;
		this->y += b.y;
		this->z += b.z;
		return *this;
	}

	Vector3 operator-(const Vector3& b) const
	{
		return Vector3(x - b.x, y - b.y, z - b.z);
	}

	Vector3& operator-=(const Vector3& b)
	{
		this->x -= b.x;
		this->y -= b.y;
		this->z -= b.z;
		return *this;
	}

	Vector3 operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	Vector3 operator*(float f) const
	{
		return Vector3(x * f, y * f, z * f);
	}

	Vector3 operator/(float f) const
	{
		float inv = 1 / f;
		return Vector3(x * inv, y * inv, z * inv);
	}

	float Magnitude() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	float MagnitudeSquared() const
	{
		return x * x + y * y + z * z;
	}

	Vector3 Normalized() const
	{
		float mag = Magnitude();

		if (mag == 0)
			return Vector3(0, 0, 0);

		return *this / mag;
	}

	float Dot(const Vector3& v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	Vector3 Cross(const Vector3& v) const
	{
		return Vector3(y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x);
	}

	float& operator[](int axis);

	static float Dot(const Vector3& a, const Vector3& b);
	static Vector3 Cross(const Vector3& a, const Vector3& b);
};

//Vector3 operator*(const gl::Matrix& mat, const Vector3& vec);s