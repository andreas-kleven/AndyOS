#include "Input.h"
#include "LocalInput.h"
#include "Player.h"
#include <AndyOS.h>
#include <map>
#include <string.h>
#include <unistd.h>

struct InputState
{
    bool keys[MAX_KEYS];
    bool prev_keys[MAX_KEYS];
    float axes[4];
    float prev_axes[4];
};

namespace Input {

std::map<int, InputState> states;

InputState &CurrentState()
{
    int id = PlayerManager::GetCurrentPlayer()->id;

    if (states.find(id) == states.end())
        states[id] = InputState();

    return states[id];
}

void UpdateLocal()
{
    for (int i = 0; i < MAX_KEYS; i++) {
        KEYCODE key = (KEYCODE)i;
        states[0].keys[key] = LocalInput::GetKey(key);
    }

    states[0].axes[AXIS_X] = LocalInput::GetAxis(AXIS_X);
    states[0].axes[AXIS_Y] = -LocalInput::GetAxis(AXIS_Y);
    states[0].axes[AXIS_SCROLLX] = LocalInput::GetAxis(AXIS_SCROLLX);
    states[0].axes[AXIS_SCROLLY] = LocalInput::GetAxis(AXIS_SCROLLY);
}

void Update(bool active)
{
    InputState &state = states[0];
    memcpy(state.prev_keys, state.keys, sizeof(state.keys));
    memcpy(state.prev_axes, state.axes, sizeof(state.axes));

    LocalInput::Update();

    if (active)
        UpdateLocal();
}

void Init()
{
    LocalInput::Init();
}

bool GetKey(KEYCODE key)
{
    InputState &state = CurrentState();
    return state.keys[key];
}

bool GetKeyDown(KEYCODE key)
{
    InputState &state = CurrentState();
    return state.keys[key] && !state.prev_keys[key];
}

bool GetKeyUp(KEYCODE key)
{
    InputState &state = CurrentState();
    return !state.keys[key] && state.prev_keys[key];
}

float GetAxis(INPUT_AXIS axis)
{
    InputState &state = CurrentState();
    return state.axes[axis] - state.prev_axes[axis];
}

} // namespace Input
