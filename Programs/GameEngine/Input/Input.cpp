#include "Input.h"
#include "Engine.h"
#include "LocalInput.h"
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

    if (PlayerManager::IsLocal())
        id = 0;

    if (states.find(id) == states.end())
        states[id] = InputState();

    return states[id];
}

void UpdateLocal()
{
    for (int i = 0; i < MAX_KEYS; i++) {
        KEYCODE key = (KEYCODE)i;
        bool prevstate = states[0].keys[key];
        bool newstate = LocalInput::GetKey(key);

        if (prevstate != newstate) {
            GEngine::game->GetNetworkManager()->SendKey(key, newstate);
        }

        states[0].keys[key] = newstate;
    }

    states[0].axes[AXIS_X] = LocalInput::GetAxis(AXIS_X);
    states[0].axes[AXIS_Y] = -LocalInput::GetAxis(AXIS_Y);
    states[0].axes[AXIS_SCROLLX] = LocalInput::GetAxis(AXIS_SCROLLX);
    states[0].axes[AXIS_SCROLLY] = LocalInput::GetAxis(AXIS_SCROLLY);
}

void Update(bool active)
{
    for (auto &pair : states) {
        InputState &state = pair.second;
        memcpy(state.prev_keys, state.keys, sizeof(state.keys));
        memcpy(state.prev_axes, state.axes, sizeof(state.axes));
    }

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
    float value = state.axes[axis] - state.prev_axes[axis];

    if (PlayerManager::IsLocal() && fabs(value) > 0) {
        GEngine::game->GetNetworkManager()->SendAxis(axis, state.axes[axis]);
    }

    return value;
}

bool SetKey(KEYCODE key, bool value)
{
    InputState &state = CurrentState();

    if (key < 0 || key >= sizeof(state.keys) / sizeof(state.keys[0]))
        return false;

    state.keys[key] = value;
    return true;
}

bool SetAxis(INPUT_AXIS axis, float value)
{
    InputState &state = CurrentState();

    if (axis < 0 || axis >= sizeof(state.axes) / sizeof(state.axes[0]))
        return false;

    state.axes[axis] = value;
    return true;
}

} // namespace Input
