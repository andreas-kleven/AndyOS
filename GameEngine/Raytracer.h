#pragma once
#include "definitions.h"
#include "Game.h"
#include "Drawing/drawing.h"

namespace gl
{
	class Raytracer
	{
	public:
		Raytracer(Game* game, GC gc);
		void Render();
	};
}