#pragma once
#include "GEngine.h"

class Thing : public GameObject
{
  public:
    Thing();

    virtual void Start();
    virtual void Update(float delta);
};
