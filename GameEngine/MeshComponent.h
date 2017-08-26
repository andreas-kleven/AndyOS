#pragma once
#include "Component.h"

class MeshComponent : public Component, public IMesh
{
public:
	Transform transform;
	int texId;

	MeshComponent();

	virtual void Update(float delta);
};