#include "GameObject.h"
#include "System.h"

GameObject::GameObject()
{
	this->parent = 0;
	this->name = "GameObject";
}

void GameObject::SetName(String name)
{
	this->name = name;
}

String GameObject::GetName()
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
	String compName = comp->GetName();
	comp->parent = this;

	if (compName.Contains("Mesh"))
	{
		meshComponents.Add((MeshComponent*)comp);
	}
	else if (compName.Contains("Rigidbody"))
	{
		int index = components.IndexOf(rigidbody);

		if(index == -1)
		{
			rigidbody = (Rigidbody*)comp;
			components.Add(rigidbody);
		}
		else
		{
			rigidbody = (Rigidbody*)comp;
			components[index] = rigidbody;
		}
		return;
	}

	components.Add(comp);
}

Component* GameObject::GetComponent(String name)
{
	for (int i = 0; i < components.Count(); i++)
	{
		Component* comp = components[i];
		if (comp->GetName() == name)
			return comp;
	}

	return 0;
}