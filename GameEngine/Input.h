#pragma once
#include "System.h"
#include "../GL/Vector3.h"

static class Input
{
public:
	static bool GetKey(KEYCODE key);
	static float GetAxis(int axis);

private:
	//Use map with names
	float axes[4];
	float last_axes[4];

	static void Update(float deltaTime);
};