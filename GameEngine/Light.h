#pragma once
#include "GameObject.h"

class LightSource : public GameObject
{
public:
	virtual float GetIntensityAtPoint(Vector point) { return 0; }
	virtual Vector GetDirectionVector(Vector point) { return Vector(); }
	virtual gl::ColRGB GetColor() { return gl::ColRGB(); }
};