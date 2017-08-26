#pragma once
#include "Component.h"

class ColliderComponent : public Component
{
public:
	Transform transform;

	ColliderComponent();

	void(*OnCollision)();

	inline bool IsSphere() { return bIsSphere; }
	inline bool IsBox() { return bIsBox; }

protected:
	bool bIsSphere;
	bool bIsBox;
};

