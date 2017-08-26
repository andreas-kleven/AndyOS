#include "PointLight.h"

PointLight::PointLight()
{
	this->intensity = 1;
	this->maxDist = 100;
	this->color = gl::ColRGB(1, 1, 1);
}

float PointLight::GetIntensityAtPoint(Vector point)
{
	Vector deltaTime = point - transform.position;
	float i = intensity - deltaTime.Magnitude() / maxDist;
	//Debug::Print("L: %f\n", i);
	return clamp(i, 0.f, intensity);
}

Vector PointLight::GetDirectionVector(Vector point)
{
	Vector deltaTime = point - transform.position;
	return deltaTime.EulerAngles();
}

gl::ColRGB PointLight::GetColor()
{
	return color;
}
