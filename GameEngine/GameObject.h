#pragma once
#include "Transform.h"
#include "Component.h"
#include "MeshComponent.h"
#include "ColliderComponent.h"
#include "PhysicsComponent.h"
#include "List.h"

class GameObject
{
public:
	Transform transform;

	GameObject* parent;
	std::List<GameObject*> children;

	std::List<Component*> components;
	std::List<MeshComponent*> meshComponents;
	std::List<ColliderComponent*> colliderComponents;
	PhysicsComponent* physicsComponent;

	GameObject();

	virtual void Start() { }
	virtual void Update(float deltaTime) { }

	void SetName(std::String name);
	std::String GetName();

	Transform GetWorldTransform();
	Vector3 GetWorldPosition();
	Quaternion GetWorldRotation();
	Vector3 GetWorldScale();

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