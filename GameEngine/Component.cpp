#include "Component.h"

Component::Component()
{
	this->parent = 0;
}

void Component::SetName(std::String name)
{
	this->name = name;
}

std::String Component::GetName()
{
	return name;
}
