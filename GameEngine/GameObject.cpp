#include "GameObject.h"
#include "System.h"

GameObject::GameObject()
{
	this->parent = 0;
	this->name = "GameObject";
}

void GameObject::SetName(std::String name)
{
	this->name = name;
}

std::String GameObject::GetName()
{
	return name;
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
	//if (parent)
	//	return parent->GetWorldRotation() + transform.rotation;

	return transform.rotation;
}

Vector3 GameObject::GetWorldScale()
{
	if (parent)
		return parent->GetWorldScale() + transform.scale;

	return transform.scale;
}

void GameObject::AddComponent(Component* comp)
{
	std::String compName = comp->GetName();
	comp->parent = this;

	if (compName.Contains("Mesh"))
	{
		meshComponents.Add((MeshComponent*)comp);
	}
	else if (compName.Contains("Collider"))
	{
		colliderComponents.Add((ColliderComponent*)comp);
	}
	else if (compName.Contains("Physics"))
	{
		int index = components.IndexOf(physicsComponent);

		if(index == -1)
		{
			physicsComponent = (PhysicsComponent*)comp;
			components.Add(physicsComponent);
		}
		else
		{
			physicsComponent = (PhysicsComponent*)comp;
			components[index] = physicsComponent;
		}
		return;
	}

	components.Add(comp);
}

Component* GameObject::GetComponent(std::String name)
{
	for (int i = 0; i < components.Count(); i++)
	{
		Component* comp = components[i];
		if (comp->GetName() == name)
			return comp;
	}

	return 0;
}