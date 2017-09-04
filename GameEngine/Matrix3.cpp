#include "Matrix3.h"

Matrix3::Matrix3()
{
	for (int i = 0; i < 9; i++)
	{
		m_elements[i] = 0;
	}
}

Matrix3::Matrix3(float elements[])
{
	for (int i = 0; i < 9; i++)
	{
		m_elements[i] = elements[i];
	}
}

Matrix3 Matrix3::Inverse()
{
	double det = m_elements[0] * (m_elements[4] * m_elements[8] - m_elements[7] * m_elements[5]) -
		m_elements[1] * (m_elements[3] * m_elements[8] - m_elements[5] * m_elements[6]) +
		m_elements[2] * (m_elements[3] * m_elements[7] - m_elements[4] * m_elements[6]);

	if (det == 0)
		return Matrix3();

	double invdet = 1 / det;

	Matrix3 minv;
	minv[0] = (m_elements[4] * m_elements[8] - m_elements[7] * m_elements[5]) * invdet;
	minv[1] = (m_elements[2] * m_elements[7] - m_elements[1] * m_elements[8]) * invdet;
	minv[2] = (m_elements[1] * m_elements[5] - m_elements[2] * m_elements[4]) * invdet;
	minv[3] = (m_elements[5] * m_elements[6] - m_elements[3] * m_elements[8]) * invdet;
	minv[4] = (m_elements[0] * m_elements[8] - m_elements[2] * m_elements[6]) * invdet;
	minv[5] = (m_elements[3] * m_elements[2] - m_elements[0] * m_elements[5]) * invdet;
	minv[6] = (m_elements[3] * m_elements[7] - m_elements[6] * m_elements[4]) * invdet;
	minv[7] = (m_elements[6] * m_elements[1] - m_elements[0] * m_elements[7]) * invdet;
	minv[8] = (m_elements[0] * m_elements[4] - m_elements[3] * m_elements[1]) * invdet;
	return minv;
}

inline Matrix3 operator+(const Matrix3& lhs, const Matrix3& rhs)
{
	Matrix3 ret = lhs;

	for (int i = 0; i < 9; i++)
	{
		ret[i] += rhs.m_elements[i];
	}

	return ret;
}

inline Matrix3 operator-(const Matrix3& lhs, const Matrix3& rhs)
{
	Matrix3 ret = lhs;

	for (int i = 0; i < 9; i++)
	{
		ret[i] -= rhs.m_elements[i];
	}

	return ret;
}

inline Matrix3 operator*(const Matrix3& lhs, const Matrix3& rhs)
{
	Matrix3 ret = lhs;
	ret *= rhs;
	return ret;
}

inline Matrix3 operator*(const Matrix3& lhs, const float& rhs)
{
	Matrix3 ret = lhs;

	for (int i = 0; i < 9; i++)
	{
		ret[i] *= rhs;
	}

	return ret;
}

inline Vector3 operator*(const Matrix3& mat, const Vector3& vec)
{
	float nx = mat.m_elements[0] * vec.x + mat.m_elements[1] * vec.y + mat.m_elements[2] * vec.z;
	float ny = mat.m_elements[3] * vec.x + mat.m_elements[4] * vec.y + mat.m_elements[5] * vec.z;
	float nz = mat.m_elements[6] * vec.x + mat.m_elements[7] * vec.y + mat.m_elements[8] * vec.z;
	return Vector3(nx, ny, nz);
}
