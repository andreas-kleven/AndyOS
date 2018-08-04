#pragma once
#include "Collider.h"
#include "Shapes/Box.h"

struct CollisionManifold
{

};

class BoxCollider : public Collider
{
public:
	BoxCollider();

	bool IsColliding(BoxCollider* other, Vector3& mtv);

	//Box GetBoundingBox();
};