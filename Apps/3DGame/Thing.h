#pragma once
#include "GameObject.h"

class Thing : public GameObject
{
public:
	Thing();

	virtual void Update(float delta);
};