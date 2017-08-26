#include "Quaternion.h"

Quaternion::Quaternion()
{
	x = 1;
	y = 0;
	z = 0;
	w = 0;
}

inline Quaternion operator*(const Quaternion& Q1, const Quaternion& Q2)
{
	Quaternion Q = Q1;
	return Q *= Q2;
	return Q;
}
