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
    virtual GameObject *CreatePlayer() { return 0; }
    virtual GameObject *CreatePlayer(const Transform &transform) { return 0; }

    Camera *GetActiveCamera();
    void SetActiveCamera(Camera *cam);

    GameObject *GetObject(const std::string &name);
    GameObject *GetObject(ObjectType type);

    inline NetworkManager *GetNetworkManager() { return network_manager; }

    template <class T> T *CreateObject(const std::string &name)
    {
        GameObject *t = new T;
        t->SetOwner(PlayerManager::GetCurrentPlayer());
        t->SetName(name);
        AddObject(t);
        return (T *)t;
    }

  private:
    Camera *active_cam;
    NetworkManager *network_manager;

    void AddObject(GameObject *object);
};
