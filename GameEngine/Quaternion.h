#pragma once
#include "../GL/Matrix4.h"
#include "Vector3.h"

using namespace gl;

struct Quaternion
{
public:
	float x;
	float y;
	float z;
	float w;

	Quaternion();

	Quaternion(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	void Rotate(Vector3 axis, float ang)
	{
		Quaternion local;
		float ang2 = ang * 0.5;
		float sa = sin(ang2);

		local.w = cos(ang2);
		local.x = axis.x * sa;
		local.y = axis.y * sa;
		local.z = axis.z * sa;

		this->operator*=(local);
	}

	float Magnitude()
	{
		return sqrt(x*x + y*y + z*z + w*w);
	}

	void Normalize()
	{
		float mag = Magnitude();
		x /= mag;
		y /= mag;
		z /= mag;
		w /= mag;
	}

	Quaternion Normalized()
	{
		Quaternion q = *this;
		float mag = q.Magnitude();
		q.x /= mag;
		q.y /= mag;
		q.z /= mag;
		q.w /= mag;
		return q;
	}

	Matrix4 ToMatrix()
	{
		//Normalize();
		Matrix4 mat;

		double sqw = w*w;
		double sqx = x*x;
		double sqy = y*y;
		double sqz = z*z;

		// invs (inverse square length) is only required if quaternion is not already normalised
		double invs = 1 / (sqx + sqy + sqz + sqw);
		mat[0] = (sqx - sqy - sqz + sqw) * invs;
		mat[5] = (-sqx + sqy - sqz + sqw) * invs;
		mat[10] = (-sqx - sqy + sqz + sqw) * invs;

		double tmp1 = x*y;
		double tmp2 = z*w;
		mat[4] = 2.0 * (tmp1 + tmp2) * invs;
		mat[1] = 2.0 * (tmp1 - tmp2) * invs;

		tmp1 = x*z;
		tmp2 = y*w;
		mat[8] = 2.0 * (tmp1 - tmp2) * invs;
		mat[2] = 2.0 * (tmp1 + tmp2) * invs;
		tmp1 = y*z;
		tmp2 = x*w;
		mat[9] = 2.0 * (tmp1 + tmp2) * invs;
		mat[6] = 2.0 * (tmp1 - tmp2) * invs;
		return mat;
	}

	Vector3 ToEuler()
	{
		Vector3 euler;

		// roll (x-axis rotation)
		float sinr = 2.0 * (w * x + y * z);
		float cosr = 1.0 - 2.0 * (x * x + y * y);
		euler.x = atan2(sinr, cosr);

		// pitch (y-axis rotation)
		double sinp = 2.0 * (w * y - z * x);

		if (abs(sinp) >= 1)
			euler.y = copysign(M_PI_2, sinp); // use 90 degrees if out of range
		else
			euler.y = asin(sinp);

		// yaw (z-axis rotation)
		double siny = 2.0 * (w * z + x * y);
		double cosy = 1.0 - 2.0 * (y * y + z * z);
		euler.z = atan2(siny, cosy);

		return euler;
	}

	void ToAxisAngle(Vector3& axis, float& ang)
	{
		if (abs(w) > 1.0f)
			Normalize();

		ang = 2.0f * (float)acos(w);
		float den = (float)sqrt(1.0 - w * w);

		if (den > 0.0001f)
		{
			axis = Vector3(x, y, z) / den;
		}
		else
		{
			axis = Vector3(1, 0, 0);
		}
	}

	Quaternion& operator-()
	{
		return Quaternion(-x, -y, -z, w);
	}

	Quaternion& operator*(const float f)
	{
		return Quaternion(x * f, y * f, z * f, w * f).Normalized();
	}

	Quaternion& operator*(const Quaternion& quat)
	{
		Quaternion q = *this;
		q *= quat;
		return q;
	}

	Vector3& operator*(Vector3& vec)
	{
		Vector3 qv = Vector3(x, y, z);
		Vector3 t = Vector3::Cross(qv, vec) * 2;
		return vec + (t * w) + Vector3::Cross(qv, t);

		//Quaternion p = Quaternion(vec.x, vec.y, vec.z, 0);
		//Quaternion qpq = *this * p * -*this;
		//return Vector3(qpq.x, qpq.y, qpq.z);
	}

	Quaternion& operator*=(const Quaternion& quat)
	{
		Quaternion q = *this;
		w = (q.w * quat.w - q.x * quat.x - q.y * quat.y - q.z * quat.z);
		x = (q.w * quat.x + q.x * quat.w + q.y * quat.z - q.z * quat.y);
		y = (q.w * quat.y - q.x * quat.z + q.y * quat.w + q.z * quat.x);
		z = (q.w * quat.z + q.x * quat.y - q.y * quat.x + q.z * quat.w);
		return *this;
	}

	static Quaternion FromEuler(const Vector3& euler);
	static Quaternion FromAxisAngle(const Vector3& axis, float ang);
};