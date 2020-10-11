#pragma once
#include "GEngine.h"

class Thing : public GameObject
{
  public:
    Thing();

    virtual void Start();
    virtual void Update(float delta);

  private:
    int mesh_index;
    MeshComponent *mesh;

    void SetMesh(int index);
};
