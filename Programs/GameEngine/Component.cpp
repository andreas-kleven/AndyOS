#include "Component.h"

Component::Component()
{
	this->parent = 0;
}

void Component::SetName(String name)
{
	this->name = name;
}

String Component::GetName()
{
	return name;
}
