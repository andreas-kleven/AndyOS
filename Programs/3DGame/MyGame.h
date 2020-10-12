#pragma once
#include "GEngine.h"

class MyGame : public Game
{
  public:
    MyGame(bool host, bool client);
    void Init();
    GameObject *CreatePlayer();
    GameObject *CreatePlayer(const Transform &transform);
};
