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

Transform GameObject::GetTransform()
{
	return transform;
}

Vector GameObject::GetPosition()
{
	return transform.position;
}

Vector GameObject::GetRotation()
{
	return transform.rotation;
}

Vector GameObject::GetScale()
{
	return transform.scale;
}

void GameObject::SetTransform(Transform trans)
{
	transform = trans;
}

void GameObject::SetPosition(Vector pos)
{
	transform.position = pos;
}

void GameObject::SetRotation(Vector rot)
{
	transform.rotation = rot;
}

void GameObject::SetScale(Vector scale)
{
	transform.scale = scale;
}

void GameObject::Translate(Vector pos)
{
	transform.position += pos;
}

void GameObject::Rotate(Vector rot)
{
	transform.rotation += rot;
}

void GameObject::Scale(Vector scale)
{
	transform.scale.x *= scale.x;
	transform.scale.y *= scale.y;
	transform.scale.z *= scale.z;
}

Transform GameObject::GetWorldTransform()
{
	return Transform(GetWorldPosition(), GetWorldRotation(), GetWorldScale());
}

Vector GameObject::GetWorldPosition()
{
	if (parent)
		return parent->GetWorldPosition() + transform.position;

	return transform.position;
}

Vector GameObject::GetWorldRotation()
{
	if (parent)
		return parent->GetWorldRotation() + transform.rotation;

	return transform.rotation;
}

Vector GameObject::GetWorldScale()
{
	if (parent)
		return parent->GetWorldScale() + transform.scale;

	return transform.scale;
}

Vector GameObject::GetRightVector()
{
	return transform.GetRightVector();
}

Vector GameObject::GetUpVector()
{
	return transform.GetUpVector();
}

Vector GameObject::GetForwardVector()
{
	return transform.GetForwardVector();
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