#include "Lighting/PointLight.h"

PointLight::PointLight()
{
    this->intensity = 1;
    this->maxDist = 100;
    this->color = Color(1, 1, 1);
}

float PointLight::GetIntensityAtPoint(Vector3 point)
{
    Vector3 dist = point - transform.position;
    float i = intensity / (1.0f + dist.MagnitudeSquared());
    return clamp(i, 0.0f, 1.0f);
}

Vector3 PointLight::GetDirectionVector(Vector3 point)
{
    Vector3 dist = point - transform.position;
    return dist.Normalized();
}

Color PointLight::GetColor()
{
    return color;
}
