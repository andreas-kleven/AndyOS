#pragma once
#include "definitions.h"
#include "Game.h"
#include "Drawing/drawing.h"

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
	ColRGB color;
};