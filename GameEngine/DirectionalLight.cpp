#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
	this->intensity = 1;
	this->color = gl::ColRGB(1, 1, 1);
}

float DirectionalLight::GetIntensityAtPoint(Vector3 point)
{
	return intensity;
}

Vector3 DirectionalLight::GetDirectionVector(Vector3 point)
{
	return transform.GetForwardVector();
}

gl::ColRGB DirectionalLight::GetColor()
{
	return color;
}
