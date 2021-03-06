#include "GameObject.h"
#include <algorithm>

GameObject::GameObject()
{
    this->netid = 0;
    this->parent = 0;
    this->rigidbody = 0;
    this->type = ObjectType::Object;
    this->name = "GameObject";
}

void GameObject::SetNetId(int netid)
{
    this->netid = netid;
}

void GameObject::SetOwner(Player *owner)
{
    this->owner = owner;
}

void GameObject::SetName(const std::string &name)
{
    this->name = name;
}

Transform GameObject::GetWorldTransform()
{
    return Transform(GetWorldPosition(), GetWorldRotation(), GetWorldScale());
}

Vector3 GameObject::GetWorldPosition()
{
    if (parent)
        return parent->GetWorldPosition() + transform.position;

    return transform.position;
}

Quaternion GameObject::GetWorldRotation()
{
    // if (parent)
    //	return parent->GetWorldRotation() + transform.rotation;

    return transform.rotation;
}

Vector3 GameObject::GetWorldScale()
{
    if (parent)
        return parent->GetWorldScale() + transform.scale;

    return transform.scale;
}

void GameObject::AddComponent(Component *comp)
{
    std::string compName = comp->GetName();
    comp->parent = this;

    if (compName.find("Mesh") != std::string::npos) {
        meshComponents.push_back((MeshComponent *)comp);
    } else if (compName.find("Rigidbody") != std::string::npos) {
        auto it = std::find(components.begin(), components.end(), rigidbody);

        if (it == components.end()) {
            rigidbody = (Rigidbody *)comp;
            components.push_back(rigidbody);
        } else {
            rigidbody = (Rigidbody *)comp;
            *it = rigidbody;
        }
        return;
    }

    components.push_back(comp);
}

Component *GameObject::GetComponent(const std::string &name)
{
    for (int i = 0; i < components.size(); i++) {
        Component *comp = components[i];
        if (comp->GetName() == name)
            return comp;
    }

    return 0;
}
