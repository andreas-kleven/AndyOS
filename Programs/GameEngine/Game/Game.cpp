#include "Game.h"
#include "GEngine.h"
#include <string>

Game::Game()
{
    active_cam = 0;
}

void Game::AddObject(GameObject *object)
{
    objects.push_back(object);

    if (!active_cam && object->GetType() == ObjectType::Camera)
        SetActiveCamera((Camera *)object);
}

Camera *Game::GetActiveCamera()
{
    return active_cam;
}

void Game::SetActiveCamera(Camera *cam)
{
    active_cam = cam;
}

GameObject *Game::GetObject(const std::string &name)
{
    for (int i = 0; i < objects.size(); i++) {
        GameObject *obj = objects[i];
        if (obj->GetName() == name)
            return obj;
    }

    return 0;
}

GameObject *Game::GetObject(ObjectType type)
{
    for (int i = 0; i < objects.size(); i++) {
        GameObject *obj = objects[i];
        if (obj->GetType() == type)
            return obj;
    }

    return 0;
}
