#pragma once
#include "Light.h"

class DirectionalLight : public LightSource
{
public:
	DirectionalLight();

	virtual float GetIntensityAtPoint(Vector3 point);
	virtual Vector3 GetDirectionVector(Vector3 point);
	virtual gl::ColRGB GetColor();

	float intensity;
	gl::ColRGB color;
};