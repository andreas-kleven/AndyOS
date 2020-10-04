#pragma once
#include "Component.h"
#include "Components/MeshComponent.h"
#include "Components/Rigidbody.h"
#include "Player.h"
#include "Transform.h"
#include <AndyOS.h>
#include <string>
#include <vector>

enum class ObjectType
{
    Object,
    Camera,
    Light
};

class GameObject
{
  public:
    int owner;
    Transform transform;

    GameObject *parent;
    std::vector<GameObject *> children;

    std::vector<Component *> components;
    std::vector<MeshComponent *> meshComponents;
    Rigidbody *rigidbody;

    GameObject();

    virtual void Start() {}
    virtual void Update(float deltaTime) {}
    virtual void LateUpdate(float deltaTime) {}

    void SetName(const std::string &name);

    Transform GetWorldTransform();
    Vector3 GetWorldPosition();
    Quaternion GetWorldRotation();
    Vector3 GetWorldScale();

    void AddComponent(Component *comp);
    Component *GetComponent(const std::string &name);

    inline ObjectType GetType() const { return this->type; }
    inline std::string GetName() const { return this->name; }

    template <class T> T *CreateComponent(const std::string &name);

  protected:
    ObjectType type;

  private:
    std::string name;
    Transform world_transform;
};

template <class T> T *GameObject::CreateComponent(const std::string &name)
{
    Component *t = new T;
    t->owner = PlayerManager::GetCurrentPlayer()->id;
    t->SetName(name);
    AddComponent(t);
    return (T *)t;
}
