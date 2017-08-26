#include "PointLight.h"

PointLight::PointLight()
{
	this->intensity = 1;
	this->max_dist = 100;
	this->color = gl::ColRGB(1, 1, 1);
}

float PointLight::GetIntensityAtPoint(Vector point)
{
	Vector delta_time = point - transform.position;
	float i = intensity - delta_time.Magnitude() / max_dist;
	//Debug::Print("L: %f\n", i);
	return clamp(i, 0.f, intensity);
}

Vector PointLight::GetDirectionVector(Vector point)
{
	Vector delta_time = point - transform.position;
	return delta_time.EulerAngles();
}

gl::ColRGB PointLight::GetColor()
{
	return color;
}
