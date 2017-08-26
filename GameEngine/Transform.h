#pragma once
#include "Vector.h"
#include "Quaternion.h"

struct Transform
{
public:
	Vector position;
	Quaternion rotation;
	Vector scale;

	Transform();
	Transform(Vector pos, Quaternion rot, Vector scale);

	void Translate(Vector pos);
	void Rotate(Quaternion rot);
	void Rotate(Vector axis, float ang);
	void Scale(Vector scale);

	Vector GetRightVector();
	Vector GetUpVector();
	Vector GetForwardVector();

	char* ToString(char* buf);
};