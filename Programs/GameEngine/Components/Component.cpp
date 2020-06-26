#include "Component.h"

Component::Component()
{
	this->parent = 0;
}

void Component::SetName(const std::string& name)
{
	this->name = name;
}

std::string Component::GetName()
{
	return name;
}
