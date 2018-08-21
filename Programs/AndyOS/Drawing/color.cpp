#include "color.h"
#include "sys/types.h"
#include "math.h"

Color Color::Red(1, 0, 0);
Color Color::Green(0, 1, 0);
Color Color::Blue(0, 0, 1);
Color Color::Cyan(0, 1, 1);
Color Color::Magenta(1, 0, 1);
Color Color::Yellow(1, 1, 0);
Color Color::Black(0, 0, 0);
Color Color::White(1, 1, 1);
Color Color::Gray(0.5, 0.5, 0.5);
Color Color::LightGray(0.8, 0.8, 0.8);
Color Color::DarkGray(0.2, 0.2, 0.2);

Color::Color()
{
	this->r = 0;
	this->g = 0;
	this->b = 0;
}

Color::Color(float r, float g, float b)
{
	this->r = clamp(r, 0.f, 1.f);
	this->g = clamp(g, 0.f, 1.f);
	this->b = clamp(b, 0.f, 1.f);
}

Color::Color(uint32 col)
{
	this->r = ((col >> 16) & 0xFF) / 255.f;
	this->g = ((col >> 8) & 0xFF) / 255.f;
	this->b = (col & 0xFF) / 255.f;
}

uint32 Color::ToInt()
{
	return (0xFF << 24)
		| (clamp((int)(r * 255), 0, 255) << 16)
		| (clamp((int)(g * 255), 0, 255) << 8)
		| clamp((int)(b * 255), 0, 255);
}

float Color::Luminosity()
{
	return 0.21 * r + 0.72 * g + 0.07 * b;
}

Color Color::operator+(Color c)
{
	return Color(r + c.r, g + c.g, b + c.b);
}

Color Color::operator*(float f)
{
	return Color(r * f, g * f, b * f);
}

Color& Color::operator*=(float f)
{
	r = r * f;
	g = g * f;
	b = b * f;
	return *this;
}
