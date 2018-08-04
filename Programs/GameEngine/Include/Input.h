#pragma once
#include "GL.h"

enum INPUT_AXIS
{
	AXIS_X,
	AXIS_Y
};

namespace Input
{
	bool GetKey(KEYCODE key);
	float GetAxis(INPUT_AXIS axis);
};