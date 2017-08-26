#pragma once
#include "Matrix.h"

using namespace gl;

class Quaternion
{
public:
	float x;
	float y;
	float z;
	float w;

	Quaternion();

	Quaternion& operator*=(const Quaternion& Q2)
	{
		Quaternion Q1 = *this;
		w = (Q1.w * Q2.w - Q1.x * Q2.x - Q1.y * Q2.y - Q1.z * Q2.z);
		x = (Q1.w * Q2.w + Q1.x * Q2.x + Q1.y * Q2.y - Q1.z * Q2.z);
		y = (Q1.w * Q2.w - Q1.x * Q2.x + Q1.y * Q2.y + Q1.z * Q2.z);
		z = (Q1.w * Q2.w + Q1.x * Q2.x - Q1.y * Q2.y + Q1.z * Q2.z);
		return *this;
	}

	Matrix ToMatrix()
	{
		Matrix mat;
		mat[0] = 1 - 2 * y*y;
		mat[1] = 2 * x*y - 2 * w*z;
		mat[2] = 2 * x*z + 2 * w*y;

		mat[4] = 2 * x*y + 2 * w*z;
		mat[5] = 1 - 2 * x*x - 2 * z*z;
		mat[6] = 2 * y*z + 2 * w*x;

		mat[8] = 2 * x*z - 2 * w*y;
		mat[9] = w*y*z - 2 * w*x;
		mat[10] = 1 - 2 * x*x - 2 * y*y;
		return mat;
	}
};

extern inline Quaternion operator*(const Quaternion& Q1, const Quaternion& Q2);
