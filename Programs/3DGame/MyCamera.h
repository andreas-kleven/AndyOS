#pragma once
#include "GEngine.h"

class MyCamera : public Camera
{
  public:
    float speed;

    MyCamera();

    virtual void Start();
    virtual void LateUpdate(float deltaTime);

  private:
    GameObject *target;
    float yaw;
    float distance;
};
