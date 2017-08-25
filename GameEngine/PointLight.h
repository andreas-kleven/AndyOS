#pragma once
#include "Light.h"

class PointLight : public LightSource
{
public:
	PointLight();

	virtual float GetIntensityAtPoint(Vector point);
	virtual Vector GetDirectionVector(Vector point);
	virtual gl::ColRGB GetColor();

	float intensity;
	float max_dist;
	gl::ColRGB color;
};