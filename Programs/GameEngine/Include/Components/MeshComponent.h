#pragma once
#include "Component.h"
#include "Model3D.h"
#include "KDTree.h"
#include "Shapes/Box.h"
#include "Lighting/Shader.h"

class MeshComponent : public Component
{
public:
	Model3D* model;
	KDTree* bvh;

	Shader shader;
	int texId;

	MeshComponent();

	virtual void Update(float delta);

	void SetModel(Model3D* model);
};