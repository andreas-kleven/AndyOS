#include "Quaternion.h"

Quaternion::Quaternion()
{
	x = 0;
	y = 0;
	z = 0;
	w = 1;
}

Quaternion Quaternion::FromEuler(const Vector3& euler)
{
	Quaternion q;
	float roll2 = euler.x * 0.5;
	float pitch2 = euler.y * 0.5;
	float yaw2 = euler.z * 0.5;

	float cr = cos(roll2);
	float sr = sin(roll2);
	float cp = cos(pitch2);
	float sp = sin(pitch2);
	float cy = cos(yaw2);
	float sy = sin(yaw2);

	q.x = cy * sr * cp - sy * cr * sp;
	q.y = cy * cr * sp + sy * sr * cp;
	q.z = sy * cr * cp - cy * sr * sp;
	q.w = cy * cr * cp + sy * sr * sp;
	return q;
}

Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float ang)
{
	Quaternion q;
	float ang2 = ang * 0.5;
	float sa = sin(ang2);

	q.w = cos(ang2);
	q.x = axis.x * sa;
	q.y = axis.y * sa;
	q.z = axis.z * sa;
	return q;
}

//https://stackoverflow.com/questions/12435671/quaternion-lookat-function
Quaternion Quaternion::LookAt(const Vector3& from, const Vector3& to)
{
	Vector3 forwardVector = (to - from).Normalized();

	float dot = Vector3::Dot(Vector3::forward, forwardVector);

	if (fabs(dot - (-1.0f)) < 0.000001f)
	{
		return Quaternion(Vector3::up, M_PI);
	}

	if (fabs(dot - (1.0f)) < 0.000001f)
	{
		return Quaternion();
	}

	float rotAngle = (float)acos(dot);
	Vector3 rotAxis = Vector3::Cross(Vector3::forward, forwardVector);
	rotAxis = rotAxis.Normalized();

	return FromAxisAngle(rotAxis, rotAngle);
}