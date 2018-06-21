#pragma once
#include "Light.h"

class PointLight : public LightSource
{
public:
	PointLight();

	virtual float GetIntensityAtPoint(Vector3 point);
	virtual Vector3 GetDirectionVector(Vector3 point);
	virtual Color GetColor();

	float intensity;
	float maxDist;
	Color color;
};