#pragma once
#include <math.h>

struct Vector2
{
public:
	float x = 0;
	float y = 0;

	Vector2();
	Vector2(float x, float y);

	bool operator==(const Vector2& b)
	{
		return x == b.x && y == b.y;
	}

	Vector2 operator+(const Vector2& b) const
	{
		return Vector2(x + b.x, y + b.y);
	}

	Vector2& operator+=(const Vector2& b)
	{
		this->x += b.x;
		this->y += b.y;
		return *this;
	}

	Vector2 operator-(const Vector2& b) const
	{
		return Vector2(x - b.x, y - b.y);
	}

	Vector2& operator-=(const Vector2& b)
	{
		this->x -= b.x;
		this->y -= b.y;
		return *this;
	}

	Vector2 operator-() const
	{
		return Vector2(-x, -y);
	}

	Vector2 operator*(float f) const
	{
		return Vector2(x * f, y * f);
	}

	Vector2 operator/(float f) const
	{
		float inv = 1 / f;
		return Vector2(x * inv, y * inv);
	}

	float Magnitude() const
	{
		return sqrt(x * x + y * y);
	}

	float MagnitudeSquared() const
	{
		return x * x + y * y;
	}

	Vector2 Normalized() const
	{
		float mag = Magnitude();

		if (mag == 0)
			return Vector2(0, 0);

		return *this / mag;
	}

	float Dot(const Vector2& v) const
	{
		return x * v.x + y * v.y;
	}

	float& operator[](int axis);

	static float Dot(const Vector2& a, const Vector2& b);
};
