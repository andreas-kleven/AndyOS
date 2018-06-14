#include "Matrix4.h"
#include "string.h"
#include "math.h"

Matrix4::Matrix4()
{
	memset(&elems, 0, sizeof(elems));
	elems[0] = 1;
	elems[5] = 1;
	elems[10] = 1;
	elems[15] = 1;
}

Matrix4::Matrix4(float elements[])
{
	memset(&elems, 0, sizeof(elems));
	for (int i = 0; i < 16; i++)
	{
		elems[i] = elements[i];
	}
}

Matrix4 Matrix4::Inverse()
{
	Matrix4 inv;

	inv[0] = elems[5] * elems[10] * elems[15] -
		elems[5] * elems[11] * elems[14] -
		elems[9] * elems[6] * elems[15] +
		elems[9] * elems[7] * elems[14] +
		elems[13] * elems[6] * elems[11] -
		elems[13] * elems[7] * elems[10];

	inv[4] = -elems[4] * elems[10] * elems[15] +
		elems[4] * elems[11] * elems[14] +
		elems[8] * elems[6] * elems[15] -
		elems[8] * elems[7] * elems[14] -
		elems[12] * elems[6] * elems[11] +
		elems[12] * elems[7] * elems[10];

	inv[8] = elems[4] * elems[9] * elems[15] -
		elems[4] * elems[11] * elems[13] -
		elems[8] * elems[5] * elems[15] +
		elems[8] * elems[7] * elems[13] +
		elems[12] * elems[5] * elems[11] -
		elems[12] * elems[7] * elems[9];

	inv[12] = -elems[4] * elems[9] * elems[14] +
		elems[4] * elems[10] * elems[13] +
		elems[8] * elems[5] * elems[14] -
		elems[8] * elems[6] * elems[13] -
		elems[12] * elems[5] * elems[10] +
		elems[12] * elems[6] * elems[9];

	inv[1] = -elems[1] * elems[10] * elems[15] +
		elems[1] * elems[11] * elems[14] +
		elems[9] * elems[2] * elems[15] -
		elems[9] * elems[3] * elems[14] -
		elems[13] * elems[2] * elems[11] +
		elems[13] * elems[3] * elems[10];

	inv[5] = elems[0] * elems[10] * elems[15] -
		elems[0] * elems[11] * elems[14] -
		elems[8] * elems[2] * elems[15] +
		elems[8] * elems[3] * elems[14] +
		elems[12] * elems[2] * elems[11] -
		elems[12] * elems[3] * elems[10];

	inv[9] = -elems[0] * elems[9] * elems[15] +
		elems[0] * elems[11] * elems[13] +
		elems[8] * elems[1] * elems[15] -
		elems[8] * elems[3] * elems[13] -
		elems[12] * elems[1] * elems[11] +
		elems[12] * elems[3] * elems[9];

	inv[13] = elems[0] * elems[9] * elems[14] -
		elems[0] * elems[10] * elems[13] -
		elems[8] * elems[1] * elems[14] +
		elems[8] * elems[2] * elems[13] +
		elems[12] * elems[1] * elems[10] -
		elems[12] * elems[2] * elems[9];

	inv[2] = elems[1] * elems[6] * elems[15] -
		elems[1] * elems[7] * elems[14] -
		elems[5] * elems[2] * elems[15] +
		elems[5] * elems[3] * elems[14] +
		elems[13] * elems[2] * elems[7] -
		elems[13] * elems[3] * elems[6];

	inv[6] = -elems[0] * elems[6] * elems[15] +
		elems[0] * elems[7] * elems[14] +
		elems[4] * elems[2] * elems[15] -
		elems[4] * elems[3] * elems[14] -
		elems[12] * elems[2] * elems[7] +
		elems[12] * elems[3] * elems[6];

	inv[10] = elems[0] * elems[5] * elems[15] -
		elems[0] * elems[7] * elems[13] -
		elems[4] * elems[1] * elems[15] +
		elems[4] * elems[3] * elems[13] +
		elems[12] * elems[1] * elems[7] -
		elems[12] * elems[3] * elems[5];

	inv[14] = -elems[0] * elems[5] * elems[14] +
		elems[0] * elems[6] * elems[13] +
		elems[4] * elems[1] * elems[14] -
		elems[4] * elems[2] * elems[13] -
		elems[12] * elems[1] * elems[6] +
		elems[12] * elems[2] * elems[5];

	inv[3] = -elems[1] * elems[6] * elems[11] +
		elems[1] * elems[7] * elems[10] +
		elems[5] * elems[2] * elems[11] -
		elems[5] * elems[3] * elems[10] -
		elems[9] * elems[2] * elems[7] +
		elems[9] * elems[3] * elems[6];

	inv[7] = elems[0] * elems[6] * elems[11] -
		elems[0] * elems[7] * elems[10] -
		elems[4] * elems[2] * elems[11] +
		elems[4] * elems[3] * elems[10] +
		elems[8] * elems[2] * elems[7] -
		elems[8] * elems[3] * elems[6];

	inv[11] = -elems[0] * elems[5] * elems[11] +
		elems[0] * elems[7] * elems[9] +
		elems[4] * elems[1] * elems[11] -
		elems[4] * elems[3] * elems[9] -
		elems[8] * elems[1] * elems[7] +
		elems[8] * elems[3] * elems[5];

	inv[15] = elems[0] * elems[5] * elems[10] -
		elems[0] * elems[6] * elems[9] -
		elems[4] * elems[1] * elems[10] +
		elems[4] * elems[2] * elems[9] +
		elems[8] * elems[1] * elems[6] -
		elems[8] * elems[2] * elems[5];

	double det = elems[0] * inv[0] + elems[1] * inv[4] + elems[2] * inv[8] + elems[3] * inv[12];

	if (det == 0)
		return Matrix4();

	inv *= 1.0 / det;
	return inv;
}

float* Matrix4::GetElementsPointer()
{
	return elems;
}

Matrix4 Matrix4::CreateTranslation(Vector4 translation)
{
	Matrix4 newMatrix = Matrix4();
	newMatrix.elems[3] = translation.x;
	newMatrix.elems[7] = translation.y;
	newMatrix.elems[11] = translation.z;
	return newMatrix;
}

Matrix4 Matrix4::CreateRotation(Vector4 rot)
{
	float sx = sin(-rot.x);
	float cx = cos(-rot.x);
	float sy = sin(-rot.y);
	float cy = cos(-rot.y);
	float sz = sin(-rot.z);
	float cz = cos(-rot.z);

	Matrix4 X;
	X.elems[5] = cx;
	X.elems[6] = -sx;
	X.elems[9] = sx;
	X.elems[10] = cx;

	Matrix4 Y;
	Y.elems[0] = cy;
	Y.elems[2] = sy;
	Y.elems[8] = -sy;
	Y.elems[10] = cy;

	Matrix4 Z;
	Z.elems[0] = cz;
	Z.elems[1] = -sz;
	Z.elems[4] = sz;
	Z.elems[5] = cz;
	return X * Y * Z;
}

Matrix4 Matrix4::CreateScale(Vector4 scale)
{
	Matrix4 newMatrix = Matrix4();
	newMatrix.elems[0] = scale.x;
	newMatrix.elems[5] = scale.y;
	newMatrix.elems[10] = scale.z;
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
	newMatrixOne.elems[0] = right.x;
	newMatrixOne.elems[1] = right.y;
	newMatrixOne.elems[2] = right.z;
	newMatrixOne.elems[4] = -up.x;
	newMatrixOne.elems[5] = -up.y;
	newMatrixOne.elems[6] = -up.z;
	newMatrixOne.elems[8] = forward.x;
	newMatrixOne.elems[9] = forward.y;
	newMatrixOne.elems[10] = forward.z;

	Matrix4 newMatrixTwo = Matrix4();
	newMatrixTwo.elems[3] = -position.x;
	newMatrixTwo.elems[7] = -position.y;
	newMatrixTwo.elems[11] = -position.z;

	Matrix4 newMatrix = newMatrixOne * newMatrixTwo;
	return newMatrix;
}

Matrix4 Matrix4::CreatePerspectiveProjection(float width, float height, float fov, float near, float far)
{
	Matrix4 newMatrix = Matrix4();
	newMatrix.elems[0] = (1.0f / tan((fov * M_PI / 180.0f) / 2.0f)) / (width / height);
	newMatrix.elems[5] = 1.0f / tan((fov * M_PI / 180.0f) / 2.0f);
	newMatrix.elems[10] = (far + near) / (near - far);
	newMatrix.elems[11] = (2.0f * far * near) / (near - far);
	newMatrix.elems[14] = -1.0f;
	newMatrix.elems[15] = 0;
	return newMatrix;
}

Matrix4 Matrix4::CreateOrthographicProjection(float left, float right, float top, float bottom, float farPlane, float nearPlane)
{
	Matrix4 newMatrix = Matrix4();
	newMatrix.elems[0] = 2.0f / (right - left);
	newMatrix.elems[3] = -((right + left) / (right - left));
	newMatrix.elems[5] = 2.0f / (top - bottom);
	newMatrix.elems[7] = -((top + bottom) / (top - bottom));
	newMatrix.elems[10] = 2.0f / (farPlane - nearPlane);
	newMatrix.elems[11] = -((farPlane + nearPlane) / (farPlane - nearPlane));
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
	float nx = mat.elems[0] * vec.x + mat.elems[1] * vec.y + mat.elems[2] * vec.z + mat.elems[3] * vec.w;
	float ny = mat.elems[4] * vec.x + mat.elems[5] * vec.y + mat.elems[6] * vec.z + mat.elems[7] * vec.w;
	float nz = mat.elems[8] * vec.x + mat.elems[9] * vec.y + mat.elems[10] * vec.z + mat.elems[11] * vec.w;
	float nw = mat.elems[12] * vec.x + mat.elems[13] * vec.y + mat.elems[14] * vec.z + mat.elems[15] * vec.w;
	return Vector4(nx, ny, nz, nw);
}