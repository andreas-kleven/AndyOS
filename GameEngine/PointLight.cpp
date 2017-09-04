#include "PointLight.h"

PointLight::PointLight()
{
	this->intensity = 1;
	this->maxDist = 100;
	this->color = gl::ColRGB(1, 1, 1);
}

float PointLight::GetIntensityAtPoint(Vector3 point)
{
	Vector3 deltaTime = point - transform.position;
	float i = intensity - deltaTime.Magnitude() / maxDist;
	//Debug::Print("L: %f\n", i);
	return clamp(i, 0.f, intensity);
}

Vector3 PointLight::GetDirectionVector(Vector3 point)
{
	Vector3 deltaTime = point - transform.position;
	return deltaTime.EulerAngles();
}

gl::ColRGB PointLight::GetColor()
{
	return color;
}
