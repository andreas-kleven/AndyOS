#pragma once
#include "Model3D.h"

enum Format3D
{
    FORMAT_OBJ,
    FORMAT_A3D
};

class ModelLoader
{
  public:
    static Model3D *LoadModel(const char *filename, Format3D format);
};