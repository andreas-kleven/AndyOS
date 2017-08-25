#pragma once
#include "Component.h"

class MeshComponent : public Component, public IMesh
{
public:
	Transform transform;
	int tex_id;

	MeshComponent();

	virtual void Update(float delta);
};