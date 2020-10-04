#include "Input.h"
#include "GUI.h"
#include "Player.h"

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
    UpdateAxes();
}

bool GetKey(KEYCODE key)
{
    if (!PlayerManager::IsLocal())
        return false;

    return gui::InputManager::GetKeyDown(key);
}

float GetAxis(INPUT_AXIS axis)
{
    if (!PlayerManager::IsLocal())
        return 0;

    return axes[axis] - last_axes[axis];
}
} // namespace Input
