#include "Input.h"

bool Input::GetKey(KEYCODE key)
{
	return get_key_down(key);
}

float Input::GetAxis(int axis)
{
	return 0;
}

void Input::Update(float deltaTime)
{
	
}
