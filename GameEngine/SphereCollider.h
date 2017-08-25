#pragma once
#include "ColliderComponent.h"

class SphereCollider : public ColliderComponent
{
public:
	float radius;

	SphereCollider();
};