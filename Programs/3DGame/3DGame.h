#pragma once
#include "GEngine.h"

class MyGame : public Game
{
  public:
    MyGame();
    void Init();
    GameObject *CreatePlayer();
    GameObject *CreatePlayer(const Transform &transform);
};
