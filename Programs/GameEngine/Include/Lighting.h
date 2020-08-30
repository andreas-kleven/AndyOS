#pragma once
#include "GameObject.h"

class LightSource : public GameObject
{
  public:
    virtual float GetIntensityAtPoint(Vector3 point) { return 0; }
    virtual Vector3 GetDirectionVector(Vector3 point) { return Vector3(); }
    virtual Color GetColor() { return Color(1, 1, 1); }
};