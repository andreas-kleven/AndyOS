#pragma once
#include <string>

class GameObject;

class Component
{
public:
	GameObject* parent;

	Component();

	virtual void Start() { }
	virtual void Update(float delta) { }

	void SetName(const std::string& name);
	std::string GetName();

private:
	std::string name;
};