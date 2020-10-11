#pragma once
#include "Player.h"
#include <string>

class GameObject;

class Component
{
  public:
    GameObject *parent;

    Component();

    virtual void Start() {}
    virtual void Update(float delta) {}
    virtual void LateUpdate(float delta) {}

    void SetOwner(Player *owner);
    void SetName(const std::string &name);

    inline int GetId() const { return this->id; }
    inline Player *GetOwner() const { return this->owner; }
    inline std::string GetName() const { return this->name; }

  private:
    int id;
    Player *owner;
    std::string name;
};
