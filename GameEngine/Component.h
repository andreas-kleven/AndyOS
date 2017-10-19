#pragma once
#include "IMesh.h"

class GameObject;

class Component
{
public:
	GameObject* parent;

	Component();

	virtual void Start() { }
	virtual void Update(float delta) { }

	void SetName(String name);
	String GetName();

private:
	String name;
};