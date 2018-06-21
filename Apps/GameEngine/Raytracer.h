#pragma once
#include <AndyOS.h>
#include "definitions.h"
#include "Game.h"

class Raytracer
{
public:
	Raytracer(Game* game, GC gc);
	void Render();
};

struct Photon
{
	Vector3 position;
	Vector3 direction;
	Vector3 surfaceNormal;
	Color color;
};