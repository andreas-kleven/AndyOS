#pragma once
#include "Scene.h"
#include <vector>

class Game
{
  public:
    // Scene* scene;
    std::vector<GameObject *> objects;

    Game();

    void AddObject(GameObject *object);

    Camera *GetActiveCamera();
    void SetActiveCamera(Camera *cam);

    GameObject *GetObject(const std::string &name);
    GameObject *GetObject(ObjectType type);

    template <class T> T *CreateObject(const std::string &name);

  private:
    Camera *active_cam;
};

// Creates an object and adds it
template <class T> T *Game::CreateObject(const std::string &name)
{
    GameObject *t = new T;
    t->SetName(name);
    AddObject(t);
    return (T *)t;
}
