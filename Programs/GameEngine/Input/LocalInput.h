#pragma once
#include "Input.h"
#include <keycodes.h>

namespace LocalInput {

void Init();
void Update();
bool GetKey(KEYCODE code);
float GetAxis(INPUT_AXIS axis);

} // namespace LocalInput
