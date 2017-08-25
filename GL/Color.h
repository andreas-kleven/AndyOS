#pragma once
#include "definitions.h"

namespace gl
{
	struct ColRGB
	{
	public:
		float r;
		float g;
		float b;

		ColRGB();
		ColRGB(float r, float g, float b);
		ColRGB::ColRGB(uint32 col);

		uint32 ToInt();
		float Luminosity();

		ColRGB operator+(ColRGB c);
		ColRGB operator*(float f);
		ColRGB operator*=(float f);
	};
}