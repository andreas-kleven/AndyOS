#include "Input.h"
#include "GUI.h"

namespace Input
{
	float axes[4];
	float last_axes[4];

	void UpdateAxes()
	{
		int dx;
		int dy;
		gui::InputManager::GetMouseDelta(dx, dy);

		axes[AXIS_X] = dx;
		axes[AXIS_Y] = -dy;
	}

	bool GetKey(KEYCODE key)
	{
		return gui::InputManager::GetKeyDown(key);
	}

	float GetAxis(INPUT_AXIS axis)
	{
		UpdateAxes();

		float delta = axes[axis] - last_axes[axis];
		last_axes[axis] = axes[axis];
		return delta;
	}
}