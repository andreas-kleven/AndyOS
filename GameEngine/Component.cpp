#include "Component.h"

void Component::SetName(std::String name)
{
	this->name = name;
}

std::String Component::GetName()
{
	return name;
}
