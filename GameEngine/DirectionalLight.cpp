#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
	this->intensity = 1;
	this->color = gl::ColRGB(1, 1, 1);
}

float DirectionalLight::GetIntensityAtPoint(Vector point)
{
	return intensity;
}

Vector DirectionalLight::GetDirectionVector(Vector point)
{
	return transform.GetForwardVector();
}

gl::ColRGB DirectionalLight::GetColor()
{
	return color;
}
