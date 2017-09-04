#include "Vector3.h"

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

Vector3 Vector3::EulerAngles()
{
	return Vector3(-cos(x) * sin(y), sin(x), cos(x) * cos(y));
}

gl::Vector4 Vector3::ToVector4()
{
	return gl::Vector4(x, y, z, 1);
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

/*inline Vector3 operator*(const gl::Matrix& mat, const Vector3& vec)
{
	float nx = mat.m_elements[0] * vec.x + mat.m_elements[1] * vec.y + mat.m_elements[2] * vec.z + mat.m_elements[3];
	float ny = mat.m_elements[4] * vec.x + mat.m_elements[5] * vec.y + mat.m_elements[6] * vec.z + mat.m_elements[7];
	float nz = mat.m_elements[8] * vec.x + mat.m_elements[9] * vec.y + mat.m_elements[10] * vec.z + mat.m_elements[11];
	return Vector3(nx, ny, nz);
}*/
