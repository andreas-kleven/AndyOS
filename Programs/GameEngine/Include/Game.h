#pragma once
#include "Net/NetManager.h"
#include "Scene.h"
#include <vector>

class Game
{
  public:
    // Scene* scene;
    std::vector<GameObject *> objects;

    Game();

    virtual void Init() {}
    virtual void CreatePlayer() {}

    Camera *GetActiveCamera();
    void SetActiveCamera(Camera *cam);

    GameObject *GetObject(const std::string &name);
    GameObject *GetObject(ObjectType type);

    template <class T> T *CreateObject(const std::string &name);

    inline NetworkManager *GetNetworkManager() { return network_manager; }

  private:
    Camera *active_cam;
    NetworkManager *network_manager;

    void AddObject(GameObject *object);
};

// Creates an object and adds it
template <class T> T *Game::CreateObject(const std::string &name)
{
    GameObject *t = new T;
    t->owner = PlayerManager::GetCurrentPlayer()->id;
    t->SetName(name);
    AddObject(t);
    return (T *)t;
}
