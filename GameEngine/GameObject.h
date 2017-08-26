#pragma once
#include "Transform.h"
#include "Component.h"
#include "MeshComponent.h"
#include "ColliderComponent.h"
#include "List.h"

class GameObject
{
public:
	Transform transform;

	GameObject* parent;
	std::List<GameObject*> children;

	std::List<Component*> components;
	std::List<MeshComponent*> mesh_components;
	std::List<ColliderComponent*> collider_components;

	GameObject();

	virtual void Start() { }
	virtual void Update(float delta_time) { }

	void SetName(std::String name);
	std::String GetName();

	Transform GetTransform();
	Vector GetPosition();
	Vector GetRotation();
	Vector GetScale();

	void SetTransform(Transform trans);
	void SetPosition(Vector pos);
	void SetRotation(Vector rot);
	void SetScale(Vector scale);

	void Translate(Vector pos);
	void Rotate(Vector rot);
	void Scale(Vector scale);

	Transform GetWorldTransform();
	Vector GetWorldPosition();
	Vector GetWorldRotation();
	Vector GetWorldScale();

	Vector GetRightVector();
	Vector GetUpVector();
	Vector GetForwardVector();

	void AddComponent(Component* comp);
	Component* GetComponent(std::String name);

	template<class T>
	T* CreateComponent(std::String name);

private:
	std::String name;
	Transform world_transform;
};

template<class T>
T* GameObject::CreateComponent(std::String name)
{
	Component* t = new T;
	t->SetName(name);
	AddComponent(t);
	return (T*)t;
}