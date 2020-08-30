#include "Transform.h"
#include <stdio.h>
#include <string.h>

Transform::Transform()
{
    this->position = Vector3();
    this->rotation = Quaternion();
    this->scale = Vector3(1, 1, 1);
}

Transform::Transform(Vector3 pos, Quaternion rot, Vector3 scale)
{
    this->position = pos;
    this->rotation = rot;
    this->scale = scale;
}

void Transform::Translate(Vector3 pos)
{
    this->position += pos;
}

void Transform::Rotate(Quaternion rot)
{
    this->rotation *= rot;
}

void Transform::Rotate(Vector3 axis, float ang)
{
    float yaw = axis.x;
    float pitch = axis.y;
    float roll = axis.z;
    float rollOver2 = roll * 0.5f;
    float sinRollOver2 = (float)sin((float)rollOver2);
    float cosRollOver2 = (float)cos((float)rollOver2);
    float pitchOver2 = pitch * 0.5f;
    float sinPitchOver2 = (float)sin((float)pitchOver2);
    float cosPitchOver2 = (float)cos((float)pitchOver2);
    float yawOver2 = yaw * 0.5f;
    float sinYawOver2 = (float)sin((float)yawOver2);
    float cosYawOver2 = (float)cos((float)yawOver2);

    Quaternion result;
    result.x =
        cosYawOver2 * cosPitchOver2 * cosRollOver2 + sinYawOver2 * sinPitchOver2 * sinRollOver2;
    result.y =
        cosYawOver2 * cosPitchOver2 * sinRollOver2 - sinYawOver2 * sinPitchOver2 * cosRollOver2;
    result.z =
        cosYawOver2 * sinPitchOver2 * cosRollOver2 + sinYawOver2 * cosPitchOver2 * sinRollOver2;
    result.w =
        sinYawOver2 * cosPitchOver2 * cosRollOver2 - cosYawOver2 * sinPitchOver2 * sinRollOver2;

    rotation *= result;

    return;

    /*Quaternion q;
    float cy = cos(axis.x / 2);
    float sy = sin(axis.x / 2);
    float cr = cos(axis.z / 2);
    float sr = sin(axis.z / 2);
    float cp = cos(axis.y / 2);
    float sp = sin(axis.y / 2);

    q.w = cy * cr * cp + sy * sr * sp;
    q.x = cy * sr * cp - sy * cr * sp;
    q.y = cy * cr * sp + sy * sr * cp;
    q.z = sy * cr * cp - cy * sr * sp;

    q *= rotation;
    rotation = q;*/
    // rotation *= Quaternion(axis.x, axis.y, axis.z, ang).Normalized();

    Quaternion q;
    ang /= 2;

    axis = axis.Normalized();
    axis = axis * sin(ang);

    q.x = axis.x;
    q.y = axis.y;
    q.z = axis.z;
    q.w = cos(ang);

    this->rotation *= q;
}

void Transform::Scale(Vector3 scale)
{
    this->scale.x *= scale.x;
    this->scale.y *= scale.y;
    this->scale.z *= scale.z;
}

Vector3 Transform::GetRightVector()
{
    return this->rotation * Vector3(1, 0, 0);
}

Vector3 Transform::GetUpVector()
{
    return this->rotation * Vector3(0, 1, 0);
}

Vector3 Transform::GetForwardVector()
{
    return this->rotation * Vector3(0, 0, 1);
}

char *Transform::ToString(char *buf)
{
    sprintf(buf, "[%f, %f, %f] [%f, %f, %f, %f] [%f, %f, %f]", position.x, position.y, position.z,
            rotation.x, rotation.y, rotation.z, rotation.w, scale.x, scale.y, scale.z);

    return buf;
}
