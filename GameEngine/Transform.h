#pragma once
#include "Vector.h"

struct Transform
{
public:
	Vector position;
	Vector rotation;
	Vector scale;

	Transform();
	Transform(Vector pos, Vector rot, Vector scale);

	Vector GetRightVector();
	Vector GetUpVector();
	Vector GetForwardVector();

	char* ToString(char* buf);
};