#pragma once
#include "Light.h"

class DirectionalLight : public LightSource
{
public:
	DirectionalLight();

	virtual float GetIntensityAtPoint(Vector point);
	virtual Vector GetDirectionVector(Vector point);
	virtual gl::ColRGB GetColor();

	float intensity;
	gl::ColRGB color;
};