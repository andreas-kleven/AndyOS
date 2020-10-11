#pragma once
#include "GEngine.h"

class MyCamera : public Camera
{
  public:
    float speed;
    GameObject *target;

    MyCamera();

    virtual void LateUpdate(float deltaTime);

  private:
    float yaw;
    float distance;
};
