#pragma once
#include "Rigidbody.h"

class Manifold
{
public:
	Manifold()
	{

	}

	Manifold(Vector3& point)
	{
		Point = point;
	}

	Vector3 Point;
	Vector3 Normal;

	float normalImpulse;
	float penetration;
	
	Rigidbody* Bod1;
	Rigidbody* Bod2;
};

static class Collision
{
public:
	float point_depth = 0;
	std::List<Manifold*> points;

	bool TestIntersection(Rigidbody& o1, Rigidbody& o2);

	void GetInterval(Rigidbody& o, Vector3 axis, float& min, float& max);

	std::List<Manifold*> CollisionPoint(Rigidbody& obj1, Rigidbody& obj2);
	void closest_Point(Vector3 p1, Vector3 q1, Vector3 p2, Vector3 q2, Vector3&c1, Vector3&c2);
	Vector3 getNormalEdge(Vector3& p1, Vector3& q1, Vector3& p2, Vector3& q2);

	bool isInside(Vector3 ip, Rigidbody obj1);
	Vector3 getNormalFace(Rigidbody& obj1, Vector3 Point);
};