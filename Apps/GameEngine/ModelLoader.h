#pragma once
#include "Model3D.h"

enum Format3D
{
	FORMAT_OBJ,
	FORMAT_A3D
};

static class ModelLoader
{
public:
	static Model3D* LoadModel(char* filename, Format3D format);
};