#pragma once
#include "GL.h"

enum INPUT_AXIS
{
	AXIS_X,
	AXIS_Y
};

class Input
{
public:
	static bool GetKey(KEYCODE key);
	static float GetAxis(INPUT_AXIS axis);

private:
	static void UpdateAxes();
};