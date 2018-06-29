#include "Input.h"
#include "GUI.h"

float axes[4];
float last_axes[4];

bool Input::GetKey(KEYCODE key)
{
	return InputParser::GetKeyDown(key);
}

float Input::GetAxis(INPUT_AXIS axis)
{
	UpdateAxes();

	float delta = axes[axis] - last_axes[axis];
	last_axes[axis] = axes[axis];
	return delta;
}

void Input::UpdateAxes()
{
	int dx;
	int dy;
	InputParser::GetMouseDelta(dx, dy);

	axes[AXIS_X] = dx;
	axes[AXIS_Y] = -dy;
}
