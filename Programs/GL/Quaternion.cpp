#include "Quaternion.h"

Quaternion::Quaternion()
{
    x = 0;
    y = 0;
    z = 0;
    w = 1;
}

Quaternion Quaternion::FromEuler(const Vector3 &euler)
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

Quaternion Quaternion::FromAxisAngle(const Vector3 &axis, float ang)
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

// https://stackoverflow.com/questions/12435671/quaternion-lookat-function
Quaternion Quaternion::LookAt(const Vector3 &from, const Vector3 &to)
{
    Vector3 forwardVector = (to - from).Normalized();

    float dot = Vector3::Dot(Vector3(0, 0, 1), forwardVector);

    if (fabs(dot - (-1.0f)) < 0.000001f) {
        return Quaternion(Vector3(0, 1, 0), M_PI);
    }

    if (fabs(dot - (1.0f)) < 0.000001f) {
        return Quaternion();
    }

    float rotAngle = (float)acos(dot);
    Vector3 rotAxis = Vector3::Cross(Vector3(0, 0, 1), forwardVector);
    rotAxis = rotAxis.Normalized();

    return FromAxisAngle(rotAxis, rotAngle);
}

// https://answers.unity.com/questions/467614/what-is-the-source-code-of-quaternionlookrotation.html
Quaternion Quaternion::LookAt(const Vector3 &from, const Vector3 &to, const Vector3 &up)
{
    Vector3 vector = (to - from).Normalized();
    Vector3 vector2 = (Vector3::Cross(up, vector)).Normalized();
    Vector3 vector3 = Vector3::Cross(vector, vector2).Normalized();
    float m00 = vector2.x;
    float m01 = vector2.y;
    float m02 = vector2.z;
    float m10 = vector3.x;
    float m11 = vector3.y;
    float m12 = vector3.z;
    float m20 = vector.x;
    float m21 = vector.y;
    float m22 = vector.z;

    float num8 = (m00 + m11) + m22;
    Quaternion quaternion;

    if (num8 > 0.f) {
        float num = (float)sqrt(num8 + 1.f);
        quaternion.w = num * 0.5f;
        num = 0.5f / num;
        quaternion.x = (m12 - m21) * num;
        quaternion.y = (m20 - m02) * num;
        quaternion.z = (m01 - m10) * num;
        return quaternion;
    }
    if ((m00 >= m11) && (m00 >= m22)) {
        float num7 = (float)sqrt(((1.f + m00) - m11) - m22);
        float num4 = 0.5f / num7;
        quaternion.x = 0.5f * num7;
        quaternion.y = (m01 + m10) * num4;
        quaternion.z = (m02 + m20) * num4;
        quaternion.w = (m12 - m21) * num4;
        return quaternion;
    }
    if (m11 > m22) {
        float num6 = (float)sqrt(((1.f + m11) - m00) - m22);
        float num3 = 0.5f / num6;
        quaternion.x = (m10 + m01) * num3;
        quaternion.y = 0.5f * num6;
        quaternion.z = (m21 + m12) * num3;
        quaternion.w = (m20 - m02) * num3;
        return quaternion;
    }
    float num5 = (float)sqrt(((1.f + m22) - m00) - m11);
    float num2 = 0.5f / num5;
    quaternion.x = (m20 + m02) * num2;
    quaternion.y = (m21 + m12) * num2;
    quaternion.z = 0.5f * num5;
    quaternion.w = (m01 - m10) * num2;
    return quaternion;
}
