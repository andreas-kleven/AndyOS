#pragma once
#include "GL.h"
#include "stdlib.h"

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

	Quaternion(const Vector3& v, float w)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = w;
	}

	void Rotate(const Vector3& axis, float ang)
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

	float Magnitude() const
	{
		return sqrt(x*x + y * y + z * z + w * w);
	}

	void Normalize()
	{
		float inv = 1 / Magnitude();
		x *= inv;
		y *= inv;
		z *= inv;
		w *= inv;
	}

	Quaternion Normalized() const
	{
		Quaternion q = *this;
		float inv = 1 / Magnitude();
		q.x *= inv;
		q.y *= inv;
		q.z *= inv;
		q.w *= inv;
		return q;
	}

	Matrix4 ToMatrix() const
	{
		//Normalize();
		Matrix4 mat;

		double sqw = w * w;
		double sqx = x * x;
		double sqy = y * y;
		double sqz = z * z;

		double invs = 1 / (sqx + sqy + sqz + sqw);
		mat[0] = (sqx - sqy - sqz + sqw) * invs;
		mat[5] = (-sqx + sqy - sqz + sqw) * invs;
		mat[10] = (-sqx - sqy + sqz + sqw) * invs;

		double tmp1 = x * y;
		double tmp2 = z * w;
		mat[4] = 2.0 * (tmp1 + tmp2) * invs;
		mat[1] = 2.0 * (tmp1 - tmp2) * invs;

		tmp1 = x * z;
		tmp2 = y * w;
		mat[8] = 2.0 * (tmp1 - tmp2) * invs;
		mat[2] = 2.0 * (tmp1 + tmp2) * invs;
		tmp1 = y * z;
		tmp2 = x * w;
		mat[9] = 2.0 * (tmp1 + tmp2) * invs;
		mat[6] = 2.0 * (tmp1 - tmp2) * invs;
		return mat;
	}

	Matrix3 ToMatrix3() const
	{
		//Normalize();
		Matrix3 mat;

		double sqw = w * w;
		double sqx = x * x;
		double sqy = y * y;
		double sqz = z * z;

		double invs = 1 / (sqx + sqy + sqz + sqw);
		mat[0] = (sqx - sqy - sqz + sqw) * invs;
		mat[4] = (-sqx + sqy - sqz + sqw) * invs;
		mat[8] = (-sqx - sqy + sqz + sqw) * invs;

		double tmp1 = x * y;
		double tmp2 = z * w;
		mat[3] = 2.0 * (tmp1 + tmp2) * invs;
		mat[1] = 2.0 * (tmp1 - tmp2) * invs;

		tmp1 = x * z;
		tmp2 = y * w;
		mat[6] = 2.0 * (tmp1 - tmp2) * invs;
		mat[2] = 2.0 * (tmp1 + tmp2) * invs;
		tmp1 = y * z;
		tmp2 = x * w;
		mat[7] = 2.0 * (tmp1 + tmp2) * invs;
		mat[5] = 2.0 * (tmp1 - tmp2) * invs;
		return mat;
	}

	Vector3 ToEuler() const
	{
		Vector3 euler;

		// roll (x-axis rotation)
		float sinr = 2.0 * (w * x + y * z);
		float cosr = 1.0 - 2.0 * (x * x + y * y);
		euler.x = atan2(sinr, cosr);

		// pitch (y-axis rotation)
		double sinp = 2.0 * (w * y - z * x);

		if (fabs(sinp) >= 1)
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
		if (fabs(w) > 1.0f)
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

	bool operator==(const Quaternion& quat) const
	{
		return x == quat.x && y == quat.y && z == quat.z && w == quat.w;
	}

	Quaternion operator-() const
	{
		return Quaternion(-x, -y, -z, w);
	}

	Quaternion operator*(const float f) const
	{
		return Quaternion(x * f, y * f, z * f, w * f).Normalized();
	}

	Quaternion operator*(const Quaternion& quat) const
	{
		Quaternion q = *this;
		q *= quat;
		return q;
	}

	Vector3 operator*(const Vector3& vec) const
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
	static Quaternion LookAt(const Vector3& from, const Vector3& to);
};