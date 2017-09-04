#include "Quaternion.h"

Quaternion::Quaternion()
{
	x = 0;
	y = 0;
	z = 0;
	w = 1;
}

inline Quaternion operator*(const Quaternion& q, const float f)
{
	return Quaternion(q.x, q.y, q.z, q.w * f);
}

inline Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion q = q1;
	q *= q2;
	return q;
}

inline Vector3 operator*(Quaternion& q, Vector3& v)
{
	Quaternion p = Quaternion(v.x, v.y, v.z, 0);
	Quaternion qpq = q * p * -q;
	return Vector3(qpq.x, qpq.y, qpq.z);
}
