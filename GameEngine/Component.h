#pragma once
#include "IMesh.h"

class GameObject;

class Component
{
public:
	GameObject* parent;

	virtual void Update(float delta) { }

	void SetName(std::String name);
	std::String GetName();

private:
	std::String name;
};