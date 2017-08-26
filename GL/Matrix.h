#pragma once
#include "Vector4.h"
#include "string.h"

namespace gl
{
	struct Matrix
	{
		float m_elements[16];

		Matrix();
		Matrix(float elements[]);

		float* GetElementsPointer();
		static Matrix CreateTranslation(Vector4 translation);
		static Matrix CreateRotation(Vector4 rot);
		static Matrix CreateScale(Vector4 scale);
		static Matrix CreateTransformation(Vector4 pos, Vector4 rot, Vector4 scale);
		static Matrix CreateView(Vector4 forward, Vector4 up, Vector4 right, Vector4 position);
		static Matrix CreatePerspectiveProjection(float width, float height, float fov, float nearPlane, float farPlane);
		static Matrix CreateOrthographicProjection(float left, float right, float top, float bottom, float farPlane, float nearPlane);

		inline Matrix& operator+=(const Matrix& rhs)
		{
			for (int i = 0; i < 16; i++)
			{
				m_elements[i] += rhs.m_elements[i];
			}
			return *this;
		}

		inline Matrix& operator-=(const Matrix& rhs)
		{
			for (int i = 0; i < 16; i++)
			{
				m_elements[i] -= rhs.m_elements[i];
			}
			return *this;
		}

		inline Matrix& operator*=(const Matrix& rhs)
		{
			float m_copy[16];
			memcpy(m_copy, m_elements, sizeof(m_elements));

			m_elements[0] = m_copy[0] * rhs.m_elements[0] + m_copy[1] * rhs.m_elements[4] + m_copy[2] * rhs.m_elements[8] + m_copy[3] * rhs.m_elements[12];
			m_elements[1] = m_copy[0] * rhs.m_elements[1] + m_copy[1] * rhs.m_elements[5] + m_copy[2] * rhs.m_elements[9] + m_copy[3] * rhs.m_elements[13];
			m_elements[2] = m_copy[0] * rhs.m_elements[2] + m_copy[1] * rhs.m_elements[6] + m_copy[2] * rhs.m_elements[10] + m_copy[3] * rhs.m_elements[14];
			m_elements[3] = m_copy[0] * rhs.m_elements[3] + m_copy[1] * rhs.m_elements[7] + m_copy[2] * rhs.m_elements[11] + m_copy[3] * rhs.m_elements[15];

			m_elements[4] = m_copy[4] * rhs.m_elements[0] + m_copy[5] * rhs.m_elements[4] + m_copy[6] * rhs.m_elements[8] + m_copy[7] * rhs.m_elements[12];
			m_elements[5] = m_copy[4] * rhs.m_elements[1] + m_copy[5] * rhs.m_elements[5] + m_copy[6] * rhs.m_elements[9] + m_copy[7] * rhs.m_elements[13];
			m_elements[6] = m_copy[4] * rhs.m_elements[2] + m_copy[5] * rhs.m_elements[6] + m_copy[6] * rhs.m_elements[10] + m_copy[7] * rhs.m_elements[14];
			m_elements[7] = m_copy[4] * rhs.m_elements[3] + m_copy[5] * rhs.m_elements[7] + m_copy[6] * rhs.m_elements[11] + m_copy[7] * rhs.m_elements[15];

			m_elements[8] = m_copy[8] * rhs.m_elements[0] + m_copy[9] * rhs.m_elements[4] + m_copy[10] * rhs.m_elements[8] + m_copy[11] * rhs.m_elements[12];
			m_elements[9] = m_copy[8] * rhs.m_elements[1] + m_copy[9] * rhs.m_elements[5] + m_copy[10] * rhs.m_elements[9] + m_copy[11] * rhs.m_elements[13];
			m_elements[10] = m_copy[8] * rhs.m_elements[2] + m_copy[9] * rhs.m_elements[6] + m_copy[10] * rhs.m_elements[10] + m_copy[11] * rhs.m_elements[14];
			m_elements[11] = m_copy[8] * rhs.m_elements[3] + m_copy[9] * rhs.m_elements[7] + m_copy[10] * rhs.m_elements[11] + m_copy[11] * rhs.m_elements[15];

			m_elements[12] = m_copy[12] * rhs.m_elements[0] + m_copy[13] * rhs.m_elements[4] + m_copy[14] * rhs.m_elements[8] + m_copy[15] * rhs.m_elements[12];
			m_elements[13] = m_copy[12] * rhs.m_elements[1] + m_copy[13] * rhs.m_elements[5] + m_copy[14] * rhs.m_elements[9] + m_copy[15] * rhs.m_elements[13];
			m_elements[14] = m_copy[12] * rhs.m_elements[2] + m_copy[13] * rhs.m_elements[6] + m_copy[14] * rhs.m_elements[10] + m_copy[15] * rhs.m_elements[14];
			m_elements[15] = m_copy[12] * rhs.m_elements[3] + m_copy[13] * rhs.m_elements[7] + m_copy[14] * rhs.m_elements[11] + m_copy[15] * rhs.m_elements[15];
			return *this;
		}

		inline float& operator[](const int index)
		{
			return m_elements[index];
		}

		inline Matrix& operator*=(const float& rhs)
		{
			for (int i = 0; i < 16; i++)
			{
				m_elements[i] *= rhs;
			}
			return *this;
		}
	};

	extern inline Matrix operator+(const Matrix& lhs, const Matrix& rhs);
	extern inline Matrix operator-(const Matrix& lhs, const Matrix& rhs);
	extern inline Matrix operator*(const Matrix& lhs, const Matrix& rhs);
	extern inline Matrix operator*(const Matrix& lhs, const float& rhs);

	extern inline Vector4 operator*(const Vector4& vec, const Matrix& mat);
}