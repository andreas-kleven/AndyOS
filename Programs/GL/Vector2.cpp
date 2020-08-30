#include "Vector2.h"
#include "Vector4.h"
#include <AndyOS.h>

Vector2::Vector2()
{
    this->x = 0;
    this->y = 0;
}

Vector2::Vector2(float x, float y)
{
    this->x = x;
    this->y = y;
}

float &Vector2::operator[](int axis)
{
    switch (axis) {
    case 0:
        return x;

    case 1:
        return y;

    default:
        // Todo: crash
        // Exceptions::ThrowException("Index out of range exception", "Vector2::operator[]");
        print("Index out of range exception");
        print("Vector2::operator[]");
        halt();
        return x;
    }
}

float Vector2::Dot(const Vector2 &a, const Vector2 &b)
{
    return a.x * b.x + a.y * b.y;
}
