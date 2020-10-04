#pragma once
#include <string>

class GameObject;

class Component
{
  public:
    int owner;
    GameObject *parent;

    Component();

    virtual void Start() {}
    virtual void Update(float delta) {}
    virtual void LateUpdate(float delta) {}

    void SetName(const std::string &name);
    std::string GetName();

  private:
    std::string name;
};
