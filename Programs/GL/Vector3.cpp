#include "Vector3.h"
#include "Vector4.h"
#include <AndyOS.h>

Vector3 Vector3::right = Vector3(1, 0, 0);
Vector3 Vector3::up = Vector3(0, 1, 0);
Vector3 Vector3::forward = Vector3(0, 0, 1);

Vector3::Vector3()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3 Vector3::EulerAngles() const
{
	return Vector3(-cos(x) * sin(y), sin(x), cos(x) * cos(y));
}

Vector4 Vector3::ToVector4(float w) const
{
	return Vector4(x, y, z, w);
}

float& Vector3::operator[](int axis)
{
	switch (axis)
	{
	case 0:
		return x;

	case 1:
		return y;

	case 2:
		return z;

	default:
		//Todo: crash
		//Exceptions::ThrowException("Index out of range exception", "Vector3::operator[]");
		print("Index out of range exception");
		print("Vector3::operator[]");
		halt();
		return x;
	}
}

float Vector3::Dot(const Vector3& a, const Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
{
	return Vector3(a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

/*Vector3 operator*(const gl::Matrix& mat, const Vector3& vec)
{
	float nx = mat.elems[0] * vec.x + mat.elems[1] * vec.y + mat.elems[2] * vec.z + mat.elems[3];
	float ny = mat.elems[4] * vec.x + mat.elems[5] * vec.y + mat.elems[6] * vec.z + mat.elems[7];
	float nz = mat.elems[8] * vec.x + mat.elems[9] * vec.y + mat.elems[10] * vec.z + mat.elems[11];
	return Vector3(nx, ny, nz);
}*/
