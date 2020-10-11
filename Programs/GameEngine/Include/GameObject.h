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

    void SetNetId(int id);
    void SetOwner(Player *owner);
    void SetName(const std::string &name);

    Transform GetWorldTransform();
    Vector3 GetWorldPosition();
    Quaternion GetWorldRotation();
    Vector3 GetWorldScale();

    void AddComponent(Component *comp);
    Component *GetComponent(const std::string &name);

    inline int GetNetId() const { return this->netid; }
    inline Player *GetOwner() const { return this->owner; }
    inline ObjectType GetType() const { return this->type; }
    inline std::string GetName() const { return this->name; }

    template <class T> T *CreateComponent(const std::string &name)
    {
        Component *t = new T;
        t->SetOwner(PlayerManager::GetCurrentPlayer());
        t->SetName(name);
        AddComponent(t);
        return (T *)t;
    }

  protected:
    ObjectType type;

  private:
    int netid;
    Player *owner;
    std::string name;
    Transform world_transform;
};
