#pragma once
#include "Light.h"

class PointLight : public LightSource
{
public:
	PointLight();

	virtual float GetIntensityAtPoint(Vector3 point);
	virtual Vector3 GetDirectionVector(Vector3 point);
	virtual ColRGB GetColor();

	float intensity;
	float maxDist;
	ColRGB color;
};