#pragma once
#include "Component.h"

class ColliderComponent : public Component
{
public:
	Transform transform;

	ColliderComponent();

	inline bool IsSphere() { return bIsSphere; }
	inline bool IsCube() { return bIsCube; }

protected:
	bool bIsSphere;
	bool bIsCube;
};

