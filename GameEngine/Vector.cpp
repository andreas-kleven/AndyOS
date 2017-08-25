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

