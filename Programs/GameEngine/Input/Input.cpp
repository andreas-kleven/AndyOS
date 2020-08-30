#include "Input.h"
#include "GUI.h"

namespace Input {
static float axes[4];
static float last_axes[4];

void UpdateAxes()
{
    int dx;
    int dy;
    gui::InputManager::GetMouseDelta(dx, dy);

    axes[AXIS_X] = dx;
    axes[AXIS_Y] = -dy;
}

void Update()
{
    memcpy(last_axes, axes, sizeof(float) * 4);
}

bool GetKey(KEYCODE key)
{
    return gui::InputManager::GetKeyDown(key);
}

float GetAxis(INPUT_AXIS axis)
{
    UpdateAxes();
    return axes[axis] - last_axes[axis];
}
} // namespace Input