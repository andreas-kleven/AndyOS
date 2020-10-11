#include "Component.h"

Component::Component()
{
    this->parent = 0;
}

void Component::SetOwner(Player *owner)
{
    this->owner = owner;
}

void Component::SetName(const std::string &name)
{
    this->name = name;
}
