#pragma once
#include "Vector3.h"
#include "string.h"

struct Matrix3
{
	float m_elements[9];

	Matrix3();
	Matrix3(float elements[]);

	Matrix3 Inverse();

	inline Matrix3& operator+=(const Matrix3& rhs)
	{
		for (int i = 0; i < 9; i++)
		{
			m_elements[i] += rhs.m_elements[i];
		}
		return *this;
	}

	inline Matrix3& operator-=(const Matrix3& rhs)
	{
		for (int i = 0; i < 9; i++)
		{
			m_elements[i] -= rhs.m_elements[i];
		}
		return *this;
	}

	inline Matrix3& operator*=(const Matrix3& rhs)
	{
		float m_copy[9];
		memcpy(m_copy, m_elements, sizeof(m_elements));

		m_elements[0] = m_copy[0] * rhs.m_elements[0] + m_copy[1] * rhs.m_elements[3] + m_copy[2] * rhs.m_elements[6];
		m_elements[1] = m_copy[0] * rhs.m_elements[1] + m_copy[1] * rhs.m_elements[4] + m_copy[2] * rhs.m_elements[7];
		m_elements[2] = m_copy[0] * rhs.m_elements[2] + m_copy[1] * rhs.m_elements[5] + m_copy[2] * rhs.m_elements[8];

		m_elements[3] = m_copy[3] * rhs.m_elements[0] + m_copy[4] * rhs.m_elements[3] + m_copy[5] * rhs.m_elements[6];
		m_elements[4] = m_copy[3] * rhs.m_elements[1] + m_copy[4] * rhs.m_elements[4] + m_copy[5] * rhs.m_elements[7];
		m_elements[5] = m_copy[3] * rhs.m_elements[2] + m_copy[4] * rhs.m_elements[5] + m_copy[5] * rhs.m_elements[8];

		m_elements[6] = m_copy[6] * rhs.m_elements[0] + m_copy[7] * rhs.m_elements[3] + m_copy[8] * rhs.m_elements[6];
		m_elements[7] = m_copy[6] * rhs.m_elements[1] + m_copy[7] * rhs.m_elements[4] + m_copy[8] * rhs.m_elements[7];
		m_elements[8] = m_copy[6] * rhs.m_elements[2] + m_copy[7] * rhs.m_elements[5] + m_copy[8] * rhs.m_elements[8];
		return *this;
	}

	inline float& operator[](const int index)
	{
		return m_elements[index];
	}

	inline Matrix3& operator*=(const float& rhs)
	{
		for (int i = 0; i < 9; i++)
		{
			m_elements[i] *= rhs;
		}
		return *this;
	}
};

extern inline Matrix3 operator+(const Matrix3& lhs, const Matrix3& rhs);
extern inline Matrix3 operator-(const Matrix3& lhs, const Matrix3& rhs);
extern inline Matrix3 operator*(const Matrix3& lhs, const Matrix3& rhs);
extern inline Matrix3 operator*(const Matrix3& lhs, const float& rhs);

extern inline Vector3 operator*(const Matrix3& mat, const Vector3& vec);
