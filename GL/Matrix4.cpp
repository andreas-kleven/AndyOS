#include "Matrix4.h"
#include "string.h"
#include "math.h"

namespace gl
{
	Matrix4::Matrix4()
	{
		memset(&m_elements, 0, sizeof(m_elements));
		m_elements[0] = 1;
		m_elements[5] = 1;
		m_elements[10] = 1;
		m_elements[15] = 1;
	}

	Matrix4::Matrix4(float elements[])
	{
		memset(&m_elements, 0, sizeof(m_elements));
		for (int i = 0; i < 16; i++)
		{
			m_elements[i] = elements[i];
		}
	}

	Matrix4 Matrix4::Inverse()
	{
		Matrix4 inv;

		inv[0] = m_elements[5] * m_elements[10] * m_elements[15] -
			m_elements[5] * m_elements[11] * m_elements[14] -
			m_elements[9] * m_elements[6] * m_elements[15] +
			m_elements[9] * m_elements[7] * m_elements[14] +
			m_elements[13] * m_elements[6] * m_elements[11] -
			m_elements[13] * m_elements[7] * m_elements[10];

		inv[4] = -m_elements[4] * m_elements[10] * m_elements[15] +
			m_elements[4] * m_elements[11] * m_elements[14] +
			m_elements[8] * m_elements[6] * m_elements[15] -
			m_elements[8] * m_elements[7] * m_elements[14] -
			m_elements[12] * m_elements[6] * m_elements[11] +
			m_elements[12] * m_elements[7] * m_elements[10];

		inv[8] = m_elements[4] * m_elements[9] * m_elements[15] -
			m_elements[4] * m_elements[11] * m_elements[13] -
			m_elements[8] * m_elements[5] * m_elements[15] +
			m_elements[8] * m_elements[7] * m_elements[13] +
			m_elements[12] * m_elements[5] * m_elements[11] -
			m_elements[12] * m_elements[7] * m_elements[9];

		inv[12] = -m_elements[4] * m_elements[9] * m_elements[14] +
			m_elements[4] * m_elements[10] * m_elements[13] +
			m_elements[8] * m_elements[5] * m_elements[14] -
			m_elements[8] * m_elements[6] * m_elements[13] -
			m_elements[12] * m_elements[5] * m_elements[10] +
			m_elements[12] * m_elements[6] * m_elements[9];

		inv[1] = -m_elements[1] * m_elements[10] * m_elements[15] +
			m_elements[1] * m_elements[11] * m_elements[14] +
			m_elements[9] * m_elements[2] * m_elements[15] -
			m_elements[9] * m_elements[3] * m_elements[14] -
			m_elements[13] * m_elements[2] * m_elements[11] +
			m_elements[13] * m_elements[3] * m_elements[10];

		inv[5] = m_elements[0] * m_elements[10] * m_elements[15] -
			m_elements[0] * m_elements[11] * m_elements[14] -
			m_elements[8] * m_elements[2] * m_elements[15] +
			m_elements[8] * m_elements[3] * m_elements[14] +
			m_elements[12] * m_elements[2] * m_elements[11] -
			m_elements[12] * m_elements[3] * m_elements[10];

		inv[9] = -m_elements[0] * m_elements[9] * m_elements[15] +
			m_elements[0] * m_elements[11] * m_elements[13] +
			m_elements[8] * m_elements[1] * m_elements[15] -
			m_elements[8] * m_elements[3] * m_elements[13] -
			m_elements[12] * m_elements[1] * m_elements[11] +
			m_elements[12] * m_elements[3] * m_elements[9];

		inv[13] = m_elements[0] * m_elements[9] * m_elements[14] -
			m_elements[0] * m_elements[10] * m_elements[13] -
			m_elements[8] * m_elements[1] * m_elements[14] +
			m_elements[8] * m_elements[2] * m_elements[13] +
			m_elements[12] * m_elements[1] * m_elements[10] -
			m_elements[12] * m_elements[2] * m_elements[9];

		inv[2] = m_elements[1] * m_elements[6] * m_elements[15] -
			m_elements[1] * m_elements[7] * m_elements[14] -
			m_elements[5] * m_elements[2] * m_elements[15] +
			m_elements[5] * m_elements[3] * m_elements[14] +
			m_elements[13] * m_elements[2] * m_elements[7] -
			m_elements[13] * m_elements[3] * m_elements[6];

		inv[6] = -m_elements[0] * m_elements[6] * m_elements[15] +
			m_elements[0] * m_elements[7] * m_elements[14] +
			m_elements[4] * m_elements[2] * m_elements[15] -
			m_elements[4] * m_elements[3] * m_elements[14] -
			m_elements[12] * m_elements[2] * m_elements[7] +
			m_elements[12] * m_elements[3] * m_elements[6];

		inv[10] = m_elements[0] * m_elements[5] * m_elements[15] -
			m_elements[0] * m_elements[7] * m_elements[13] -
			m_elements[4] * m_elements[1] * m_elements[15] +
			m_elements[4] * m_elements[3] * m_elements[13] +
			m_elements[12] * m_elements[1] * m_elements[7] -
			m_elements[12] * m_elements[3] * m_elements[5];

		inv[14] = -m_elements[0] * m_elements[5] * m_elements[14] +
			m_elements[0] * m_elements[6] * m_elements[13] +
			m_elements[4] * m_elements[1] * m_elements[14] -
			m_elements[4] * m_elements[2] * m_elements[13] -
			m_elements[12] * m_elements[1] * m_elements[6] +
			m_elements[12] * m_elements[2] * m_elements[5];

		inv[3] = -m_elements[1] * m_elements[6] * m_elements[11] +
			m_elements[1] * m_elements[7] * m_elements[10] +
			m_elements[5] * m_elements[2] * m_elements[11] -
			m_elements[5] * m_elements[3] * m_elements[10] -
			m_elements[9] * m_elements[2] * m_elements[7] +
			m_elements[9] * m_elements[3] * m_elements[6];

		inv[7] = m_elements[0] * m_elements[6] * m_elements[11] -
			m_elements[0] * m_elements[7] * m_elements[10] -
			m_elements[4] * m_elements[2] * m_elements[11] +
			m_elements[4] * m_elements[3] * m_elements[10] +
			m_elements[8] * m_elements[2] * m_elements[7] -
			m_elements[8] * m_elements[3] * m_elements[6];

		inv[11] = -m_elements[0] * m_elements[5] * m_elements[11] +
			m_elements[0] * m_elements[7] * m_elements[9] +
			m_elements[4] * m_elements[1] * m_elements[11] -
			m_elements[4] * m_elements[3] * m_elements[9] -
			m_elements[8] * m_elements[1] * m_elements[7] +
			m_elements[8] * m_elements[3] * m_elements[5];

		inv[15] = m_elements[0] * m_elements[5] * m_elements[10] -
			m_elements[0] * m_elements[6] * m_elements[9] -
			m_elements[4] * m_elements[1] * m_elements[10] +
			m_elements[4] * m_elements[2] * m_elements[9] +
			m_elements[8] * m_elements[1] * m_elements[6] -
			m_elements[8] * m_elements[2] * m_elements[5];

		double det = m_elements[0] * inv[0] + m_elements[1] * inv[4] + m_elements[2] * inv[8] + m_elements[3] * inv[12];

		if (det == 0)
			return Matrix4();

		inv *= 1.0 / det;
		return inv;
	}

	float* Matrix4::GetElementsPointer()
	{
		return m_elements;
	}

	Matrix4 Matrix4::CreateTranslation(Vector4 translation)
	{
		Matrix4 newMatrix = Matrix4();
		newMatrix.m_elements[3] = translation.x;
		newMatrix.m_elements[7] = translation.y;
		newMatrix.m_elements[11] = translation.z;
		return newMatrix;
	}

	Matrix4 Matrix4::CreateRotation(Vector4 rot)
	{
		Matrix4 X;
		X.m_elements[5] = cos(-rot.x);
		X.m_elements[6] = -sin(-rot.x);
		X.m_elements[9] = sin(-rot.x);
		X.m_elements[10] = cos(-rot.x);

		Matrix4 Y;
		Y.m_elements[0] = cos(-rot.y);
		Y.m_elements[2] = sin(-rot.y);
		Y.m_elements[8] = -sin(-rot.y);
		Y.m_elements[10] = cos(-rot.y);

		Matrix4 Z;
		Z.m_elements[0] = cos(-rot.z);
		Z.m_elements[1] = -sin(-rot.z);
		Z.m_elements[4] = sin(-rot.z);
		Z.m_elements[5] = cos(-rot.z);
		return X * Y * Z;
	}

	Matrix4 Matrix4::CreateScale(Vector4 scale)
	{
		Matrix4 newMatrix = Matrix4();
		newMatrix.m_elements[0] = scale.x;
		newMatrix.m_elements[5] = scale.y;
		newMatrix.m_elements[10] = scale.z;
		return newMatrix;
	}

	Matrix4 Matrix4::CreateTransformation(Vector4 pos, Vector4 rot, Vector4 scale)
	{
		Matrix4 T = CreateTranslation(pos);
		Matrix4 R = CreateRotation(rot);
		Matrix4 S = CreateScale(scale);
		return T * R * S;
	}

	Matrix4 Matrix4::CreateView(Vector4 forward, Vector4 up, Vector4 right, Vector4 position)
	{
		Matrix4 newMatrixOne = Matrix4();
		newMatrixOne.m_elements[0] = right.x;
		newMatrixOne.m_elements[1] = right.y;
		newMatrixOne.m_elements[2] = right.z;
		newMatrixOne.m_elements[4] = -up.x;
		newMatrixOne.m_elements[5] = -up.y;
		newMatrixOne.m_elements[6] = -up.z;
		newMatrixOne.m_elements[8] = forward.x;
		newMatrixOne.m_elements[9] = forward.y;
		newMatrixOne.m_elements[10] = forward.z;

		Matrix4 newMatrixTwo = Matrix4();
		newMatrixTwo.m_elements[3] = -position.x;
		newMatrixTwo.m_elements[7] = -position.y;
		newMatrixTwo.m_elements[11] = -position.z;

		Matrix4 newMatrix = newMatrixOne * newMatrixTwo;
		return newMatrix;
	}

	Matrix4 Matrix4::CreatePerspectiveProjection(float width, float height, float fov, float near, float far)
	{
		Matrix4 newMatrix = Matrix4();
		newMatrix.m_elements[0] = (1.0f / tan((fov * M_PI / 180.0f) / 2.0f)) / (width / height);
		newMatrix.m_elements[5] = 1.0f / tan((fov * M_PI / 180.0f) / 2.0f);
		newMatrix.m_elements[10] = (far + near) / (near - far);
		newMatrix.m_elements[11] = (2.0f * far * near) / (near - far);
		newMatrix.m_elements[14] = -1.0f;
		newMatrix.m_elements[15] = 0;
		return newMatrix;
	}

	Matrix4 Matrix4::CreateOrthographicProjection(float left, float right, float top, float bottom, float farPlane, float nearPlane)
	{
		Matrix4 newMatrix = Matrix4();
		newMatrix.m_elements[0] = 2.0f / (right - left);
		newMatrix.m_elements[3] = -((right + left) / (right - left));
		newMatrix.m_elements[5] = 2.0f / (top - bottom);
		newMatrix.m_elements[7] = -((top + bottom) / (top - bottom));
		newMatrix.m_elements[10] = 2.0f / (farPlane - nearPlane);
		newMatrix.m_elements[11] = -((farPlane + nearPlane) / (farPlane - nearPlane));
		return newMatrix;
	}


	inline Matrix4 operator+(const Matrix4& lhs, const Matrix4& rhs)
	{
		Matrix4 newMatrix = Matrix4(lhs);
		newMatrix += rhs;
		return newMatrix;
	}

	inline Matrix4 operator-(const Matrix4& lhs, const Matrix4& rhs)
	{
		Matrix4 newMatrix = Matrix4(lhs);
		newMatrix -= rhs;
		return newMatrix;
	}

	inline Matrix4 operator*(const Matrix4& lhs, const Matrix4& rhs)
	{
		Matrix4 newMatrix = Matrix4(lhs);
		newMatrix *= rhs;
		return newMatrix;
	}

	inline Matrix4 operator*(const Matrix4& lhs, const float& rhs)
	{
		Matrix4 newMatrix = Matrix4(lhs);
		newMatrix *= rhs;
		return newMatrix;
	}


	inline Vector4 operator*(const Matrix4& mat, const Vector4& vec)
	{
		float nx = mat.m_elements[0] * vec.x + mat.m_elements[1] * vec.y + mat.m_elements[2] * vec.z + mat.m_elements[3] * vec.w;
		float ny = mat.m_elements[4] * vec.x + mat.m_elements[5] * vec.y + mat.m_elements[6] * vec.z + mat.m_elements[7] * vec.w;
		float nz = mat.m_elements[8] * vec.x + mat.m_elements[9] * vec.y + mat.m_elements[10] * vec.z + mat.m_elements[11] * vec.w;
		float nw = mat.m_elements[12] * vec.x + mat.m_elements[13] * vec.y + mat.m_elements[14] * vec.z + mat.m_elements[15] * vec.w;
		return Vector4(nx, ny, nz, nw);
	}
}