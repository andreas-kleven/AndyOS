#pragma once
#include <sys/types.h>

struct Color
{
	float r;
	float g;
	float b;

	Color();
	Color(float r, float g, float b);
	Color(uint32_t col);

	uint32_t ToInt();
	float Luminosity();

	Color operator+(Color c);
	Color operator*(float f);
	Color& operator*=(float f);

	static Color Red;
	static Color Green;
	static Color Blue;
	static Color Cyan;
	static Color Magenta;
	static Color Yellow;
	static Color Black;
	static Color White;
	static Color Gray;
	static Color LightGray;
	static Color DarkGray;
};
