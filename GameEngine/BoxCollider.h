#pragma once
#include "ColliderComponent.h"
#include "Box.h"

class BoxCollider : public ColliderComponent
{
public:
	BoxCollider();

	bool IsColliding(BoxCollider* other);

	//Box GetBoundingBox();
};