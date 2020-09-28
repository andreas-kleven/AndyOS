#pragma once
#include "GL/Vector3.h"

namespace Math {

inline float Clamp(float val, float min, float max)
{
    if (val < min)
        return min;

    if (val > max)
        return max;

    return val;
}

inline float Min(float a, float b)
{
    if (a < b)
        return a;
    return b;
}

inline float Max(float a, float b)
{
    if (a > b)
        return a;
    return b;
}

inline float Lerp(float a, float b, float alpha)
{
    alpha = Clamp(alpha, 0.f, 1.f);
    return (a - b) * alpha + b;
}

inline Vector3 Lerp(const Vector3 &a, const Vector3 &b, float alpha)
{
    return Vector3(Lerp(a.x, b.x, alpha), Lerp(a.y, b.y, alpha), Lerp(a.z, b.z, alpha));
}

} // namespace Math
