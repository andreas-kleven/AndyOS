#pragma once
#include "Component.h"
#include "Components/MeshComponent.h"
#include "Components/Rigidbody.h"
#include "Transform.h"
#include <AndyOS.h>
#include <string>
#include <vector>

class GameObject
{
  public:
    Transform transform;

    GameObject *parent;
    std::vector<GameObject *> children;

    std::vector<Component *> components;
    std::vector<MeshComponent *> meshComponents;
    Rigidbody *rigidbody;

    GameObject();

    virtual void Start() {}
    virtual void Update(float deltaTime) {}

    void SetName(const std::string &name);
    std::string GetName();

    Transform GetWorldTransform();
    Vector3 GetWorldPosition();
    Quaternion GetWorldRotation();
    Vector3 GetWorldScale();

    void AddComponent(Component *comp);
    Component *GetComponent(const std::string &name);

    template <class T> T *CreateComponent(const std::string &name);

  private:
    std::string name;
    Transform world_transform;
};

template <class T> T *GameObject::CreateComponent(const std::string &name)
{
    Component *t = new T;
    t->SetName(name);
    AddComponent(t);
    return (T *)t;
}