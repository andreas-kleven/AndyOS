#pragma once
#include "../GL/Matrix.h"
#include "Vector.h"

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

	void Rotate(Vector axis, float ang)
	{
		Quaternion local;
		local.w = cos(ang / 2);
		local.x = axis.x * sin(ang / 2);
		local.y = axis.y * sin(ang / 2);
		local.z = axis.z * sin(ang / 2);

		this->operator*=(local);
		//w += ang;
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

	Matrix ToMatrix()
	{
		//Normalize();
		Matrix mat;

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

	Vector ToEuler()
	{
		return Vector();
	}

	Quaternion& operator-()
	{
		return Quaternion(-x, -y, -z, w);
	}

	Quaternion& operator*=(const Quaternion& q2)
	{
		Quaternion q1 = *this;
		w = (q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
		x = (q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y);
		y = (q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x);
		z = (q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w);
		return *this;
	}

	//static Quaternion FromEuler(Vector vec);
	//static Quaternion AngleAxis(Vector vec);
};

extern inline Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
extern inline Vector operator*(Quaternion& q, Vector& vec);
