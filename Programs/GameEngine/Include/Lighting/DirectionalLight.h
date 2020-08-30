#pragma once
#include "Lighting.h"

class DirectionalLight : public LightSource
{
  public:
    DirectionalLight();

    virtual float GetIntensityAtPoint(Vector3 point);
    virtual Vector3 GetDirectionVector(Vector3 point);
    virtual Color GetColor();

    float intensity;
    Color color;
};