#pragma once
#include <keycodes.h>

enum INPUT_AXIS
{
    AXIS_X,
    AXIS_Y,
    AXIS_SCROLLX,
    AXIS_SCROLLY,
};

namespace Input {
void Init();
void Update(bool active);
bool GetKey(KEYCODE key);
bool GetKeyDown(KEYCODE key);
bool GetKeyUp(KEYCODE key);
float GetAxis(INPUT_AXIS axis);
}; // namespace Input
