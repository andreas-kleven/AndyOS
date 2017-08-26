#include "Vector.h"

Vector::Vector()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Vector::Vector(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector Vector::EulerAngles()
{
	return Vector(-cos(x) * sin(y), sin(x), cos(x) * cos(y));
}

gl::Vector4 Vector::ToVector4()
{
	return gl::Vector4(x, y, z, 1);
}

float Vector::Dot(const Vector& a, const Vector& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector Vector::Cross(const Vector & a, const Vector & b)
{
	return Vector(a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}
