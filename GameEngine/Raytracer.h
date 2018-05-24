#pragma once
#include "definitions.h"
#include "Game.h"
#include "Drawing/drawing.h"

namespace gl
{
	class Raytracer
	{
	public:
		Raytracer(GC gc);
		void Render(Game* game);
	};
}