#include "Camera.h"

Camera::Camera()
{
    this->type = ObjectType::Camera;
}

void Camera::RotateEuler(const Vector3 &rot)
{
    euler_angles += rot;
    transform.rotation = Quaternion::FromEuler(euler_angles);
}
