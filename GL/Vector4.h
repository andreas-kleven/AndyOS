#pragma once
#include "math.h"

namespace gl
{
	struct Vector4
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Vector4();
		Vector4(float x, float y, float z, float w);

		//Does not negate w
		Vector4 operator- ()
		{
			return Vector4(-x, -y, -z, w);
		}

		Vector4 operator* (float f)
		{
			return Vector4(x * f, y * f, z * f, w);
		}

		Vector4 operator/ (float f)
		{
			return Vector4(x / f, y / f, z / f, w);
		}

		float Magnitude()
		{
			return sqrt(x * x + y * y + z * z);
		}

		Vector4 Normalized()
		{
			return *this / Magnitude();
		}

		float DotProduct(Vector4 v)
		{
			return x * v.x + y * v.y + z * v.z;
		}
	};
}