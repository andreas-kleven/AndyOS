#pragma once
#include "Component.h"
#include "Shader.h"

class MeshComponent : public Component, public IMesh
{
public:
	Transform transform;
	int texId;
	Shader shader;

	MeshComponent();

	virtual void Update(float delta);
};