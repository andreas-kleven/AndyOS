#include "Camera.h"

Camera::Camera()
{
	speed = 1;
}

void Camera::RotateEuler(const Vector3& rot)
{
	euler_angles += rot;
	transform.rotation = Quaternion::FromEuler(euler_angles);
}
