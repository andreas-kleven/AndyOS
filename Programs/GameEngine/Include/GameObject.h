#pragma once
#include <AndyOS.h>
#include <andyos/string.h>
#include "Transform.h"
#include "Component.h"
#include "Components/MeshComponent.h"
#include "Components/Rigidbody.h"

class GameObject
{
public:
	Transform transform;

	GameObject* parent;
	List<GameObject*> children;

	List<Component*> components;
	List<MeshComponent*> meshComponents;
	Rigidbody* rigidbody;

	GameObject();

	virtual void Start() { }
	virtual void Update(float deltaTime) { }

	void SetName(String name);
	String GetName();

	Transform GetWorldTransform();
	Vector3 GetWorldPosition();
	Quaternion GetWorldRotation();
	Vector3 GetWorldScale();

	void AddComponent(Component* comp);
	Component* GetComponent(String name);

	template<class T>
	T* CreateComponent(String name);

private:
	String name;
	Transform world_transform;
};

template<class T>
T* GameObject::CreateComponent(String name)
{
	Component* t = new T;
	t->SetName(name);
	AddComponent(t);
	return (T*)t;
}