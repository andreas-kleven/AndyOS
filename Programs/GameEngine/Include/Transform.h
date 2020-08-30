#pragma once
#include "GL.h"

struct Transform
{
  public:
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;

    Transform();
    Transform(Vector3 pos, Quaternion rot, Vector3 scale);

    void Translate(Vector3 pos);
    void Rotate(Quaternion rot);
    void Rotate(Vector3 axis, float ang);
    void Scale(Vector3 scale);

    Vector3 GetRightVector();
    Vector3 GetUpVector();
    Vector3 GetForwardVector();

    char *ToString(char *buf);
};