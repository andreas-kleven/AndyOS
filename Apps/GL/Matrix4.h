#pragma once
#include "Vector4.h"
#include "string.h"

struct Matrix4
{
	float elems[16];

	Matrix4();
	Matrix4(float elements[]);

	Matrix4 Inverse();

	float* GetElementsPointer();
	static Matrix4 CreateTranslation(Vector4 translation);
	static Matrix4 CreateRotation(Vector4 rot);
	static Matrix4 CreateScale(Vector4 scale);
	static Matrix4 CreateTransformation(Vector4 pos, Vector4 rot, Vector4 scale);
	static Matrix4 CreateView(Vector4 forward, Vector4 up, Vector4 right, Vector4 position);
	static Matrix4 CreatePerspectiveProjection(float width, float height, float fov, float nearPlane, float farPlane);
	static Matrix4 CreateOrthographicProjection(float left, float right, float top, float bottom, float farPlane, float nearPlane);

	inline Matrix4& operator+=(const Matrix4& rhs)
	{
		for (int i = 0; i < 16; i++)
		{
			elems[i] += rhs.elems[i];
		}
		return *this;
	}

	inline Matrix4& operator-=(const Matrix4& rhs)
	{
		for (int i = 0; i < 16; i++)
		{
			elems[i] -= rhs.elems[i];
		}
		return *this;
	}

	inline Matrix4& operator*=(const Matrix4& rhs)
	{
		float m_copy[16];
		memcpy(m_copy, elems, sizeof(elems));

		elems[0] = m_copy[0] * rhs.elems[0] + m_copy[1] * rhs.elems[4] + m_copy[2] * rhs.elems[8] + m_copy[3] * rhs.elems[12];
		elems[1] = m_copy[0] * rhs.elems[1] + m_copy[1] * rhs.elems[5] + m_copy[2] * rhs.elems[9] + m_copy[3] * rhs.elems[13];
		elems[2] = m_copy[0] * rhs.elems[2] + m_copy[1] * rhs.elems[6] + m_copy[2] * rhs.elems[10] + m_copy[3] * rhs.elems[14];
		elems[3] = m_copy[0] * rhs.elems[3] + m_copy[1] * rhs.elems[7] + m_copy[2] * rhs.elems[11] + m_copy[3] * rhs.elems[15];

		elems[4] = m_copy[4] * rhs.elems[0] + m_copy[5] * rhs.elems[4] + m_copy[6] * rhs.elems[8] + m_copy[7] * rhs.elems[12];
		elems[5] = m_copy[4] * rhs.elems[1] + m_copy[5] * rhs.elems[5] + m_copy[6] * rhs.elems[9] + m_copy[7] * rhs.elems[13];
		elems[6] = m_copy[4] * rhs.elems[2] + m_copy[5] * rhs.elems[6] + m_copy[6] * rhs.elems[10] + m_copy[7] * rhs.elems[14];
		elems[7] = m_copy[4] * rhs.elems[3] + m_copy[5] * rhs.elems[7] + m_copy[6] * rhs.elems[11] + m_copy[7] * rhs.elems[15];

		elems[8] = m_copy[8] * rhs.elems[0] + m_copy[9] * rhs.elems[4] + m_copy[10] * rhs.elems[8] + m_copy[11] * rhs.elems[12];
		elems[9] = m_copy[8] * rhs.elems[1] + m_copy[9] * rhs.elems[5] + m_copy[10] * rhs.elems[9] + m_copy[11] * rhs.elems[13];
		elems[10] = m_copy[8] * rhs.elems[2] + m_copy[9] * rhs.elems[6] + m_copy[10] * rhs.elems[10] + m_copy[11] * rhs.elems[14];
		elems[11] = m_copy[8] * rhs.elems[3] + m_copy[9] * rhs.elems[7] + m_copy[10] * rhs.elems[11] + m_copy[11] * rhs.elems[15];

		elems[12] = m_copy[12] * rhs.elems[0] + m_copy[13] * rhs.elems[4] + m_copy[14] * rhs.elems[8] + m_copy[15] * rhs.elems[12];
		elems[13] = m_copy[12] * rhs.elems[1] + m_copy[13] * rhs.elems[5] + m_copy[14] * rhs.elems[9] + m_copy[15] * rhs.elems[13];
		elems[14] = m_copy[12] * rhs.elems[2] + m_copy[13] * rhs.elems[6] + m_copy[14] * rhs.elems[10] + m_copy[15] * rhs.elems[14];
		elems[15] = m_copy[12] * rhs.elems[3] + m_copy[13] * rhs.elems[7] + m_copy[14] * rhs.elems[11] + m_copy[15] * rhs.elems[15];
		return *this;
	}

	inline float& operator[](const int index)
	{
		return elems[index];
	}

	inline Matrix4& operator*=(const float& rhs)
	{
		for (int i = 0; i < 16; i++)
		{
			elems[i] *= rhs;
		}
		return *this;
	}
};

extern inline Matrix4 operator+(const Matrix4& lhs, const Matrix4& rhs);
extern inline Matrix4 operator-(const Matrix4& lhs, const Matrix4& rhs);
extern inline Matrix4 operator*(const Matrix4& lhs, const Matrix4& rhs);
extern inline Matrix4 operator*(const Matrix4& lhs, const float& rhs);

extern inline Vector4 operator*(const Matrix4& mat, const Vector4& vec);