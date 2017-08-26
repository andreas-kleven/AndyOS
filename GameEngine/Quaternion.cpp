#include "Quaternion.h"

Quaternion::Quaternion()
{
	x = 1;
	y = 0;
	z = 0;
	w = 0;
}

inline Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion q = q1;
	return q *= q2;
	return q;
}

inline Vector operator*(Quaternion& q, Vector& v)
{
	Vector k(q.x, q.y, q.z);
	float t = q.w;

	return v * cos(t)
		+ Vector::Cross(k, v) * sin(t) 
		+ k * Vector::Dot(k, v) * (1 - cos(t));
}
