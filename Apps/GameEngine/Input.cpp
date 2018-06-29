#include "Input.h"
#include "GUI.h"

bool Input::GetKey(KEYCODE key)
{
	return InputParser::GetKeyDown(key);
}

float Input::GetAxis(int axis)
{
	return 0;
}

void Input::Update(float deltaTime)
{
	
}
