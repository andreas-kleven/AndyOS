#pragma once
#include <AndyOS.h>
#include "sys/types.h"
#include "Game.h"

class Raytracer
{
public:
	Raytracer(GC gc);
	void Render();
};

struct Photon
{
	Vector3 position;
	Vector3 direction;
	Vector3 surfaceNormal;
	Color color;
};