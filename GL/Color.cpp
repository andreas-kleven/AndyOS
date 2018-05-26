#include "Color.h"
#include "definitions.h"
#include "math.h"

namespace gl
{
	ColRGB::ColRGB()
	{
		this->r = 0;
		this->g = 0;
		this->b = 0;
	}

	ColRGB::ColRGB(float r, float g, float b)
	{
		this->r = clamp(r, 0.f, 1.f);
		this->g = clamp(g, 0.f, 1.f);
		this->b = clamp(b, 0.f, 1.f);
	}

	ColRGB::ColRGB(uint32 col)
	{
		this->r = ((col >> 16) & 0xFF) / 255.f;
		this->g = ((col >> 8) & 0xFF) / 255.f;
		this->b = (col & 0xFF) / 255.f;
	}

	uint32 ColRGB::ToInt()
	{
		return (0xFF << 24)
			| (clamp((int)(r * 255), 0, 255) << 16)
			| (clamp((int)(g * 255), 0, 255) << 8)
			| clamp((int)(b * 255), 0, 255);
	}

	float ColRGB::Luminosity()
	{
		return 0.21 * r + 0.72 * g + 0.07 * b;
	}

	ColRGB ColRGB::operator+(ColRGB c)
	{
		return ColRGB(r + c.r, g + c.g, b + c.b);
	}

	ColRGB ColRGB::operator*(float f)
	{
		return ColRGB(r * f, g * f, b * f);
	}

	ColRGB ColRGB::operator*=(float f)
	{
		r = r * f;
		g = g * f;
		b = b * f;
		return *(this);
	}
}