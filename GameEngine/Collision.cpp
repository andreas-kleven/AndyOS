#include "Collision.h"
#include "GameObject.h"
#include "math.h"
#include "debug.h"

#define EPSILON 0.00001f
#define DEPSILON 0.05f

/*void GetInterval(Rigidbody o, Vector3 axis, float &min, float &max)
{
	min = max = Vector3::Dot(axis, o.collider->GetVertex(0));

	for (int i = 1; i < 8; i++)
	{
		float value = Vector3::Dot(axis, o.collider->GetVertex(i));
		min = ::min(min, value);
		max = ::max(max, value);
	}
}*/

float distance(float minA, float maxA, float minB, float maxB)
{
	if (minA < minB) return minB - maxA;
	else return minA - maxB;
}

Vector3 transform(Rigidbody& body, Vector3 point)
{
	Transform transform = body.parent->GetWorldTransform();
	point = Vector3(point.x * transform.scale.x, point.y * transform.scale.y, point.z * transform.scale.z);
	return transform.rotation * point + transform.position;
}

Vector3 transform2(Rigidbody& body, Vector3 point)
{
	Transform transform = body.parent->GetWorldTransform();
	Vector3 pos = point - transform.position;
	//pos = Vector3(pos.x / transform.scale.x, pos.y / transform.scale.y, pos.z / transform.scale.z);
	return -transform.rotation * pos;
}

bool Collision::TestIntersection(Rigidbody& o1, Rigidbody& o2, Vector3* mtv, Manifold*& col, int& count)
{
	float min1, max1, min2, max2;

	float lowest = 1e100;
	Vector3 la;

	Vector3 axes[15];
	axes[0] = o1.parent->GetWorldRotation() * Vector3(1, 0, 0);
	axes[1] = o1.parent->GetWorldRotation() * Vector3(0, 1, 0);
	axes[2] = o1.parent->GetWorldRotation() * Vector3(0, 0, 1);
	axes[3] = o2.parent->GetWorldRotation() * Vector3(1, 0, 0);
	axes[4] = o2.parent->GetWorldRotation() * Vector3(0, 1, 0);
	axes[5] = o2.parent->GetWorldRotation() * Vector3(0, 0, 1);

	//axes[6] = Vector3::Cross(axes[0], axes[3]);
	//axes[7] = Vector3::Cross(axes[0], axes[4]);
	//axes[8] = Vector3::Cross(axes[0], axes[5]);
	//
	//axes[9] = Vector3::Cross(axes[1], axes[3]);
	//axes[10] = Vector3::Cross(axes[1], axes[4]);
	//axes[11] = Vector3::Cross(axes[1], axes[5]);
	//
	//axes[12] = Vector3::Cross(axes[2], axes[3]);
	//axes[13] = Vector3::Cross(axes[2], axes[4]);
	//axes[14] = Vector3::Cross(axes[2], axes[5]);

	for (int i = 0; i < 6; i++)
	{
		Vector3& axis = axes[i];

		GetInterval(o1, axis, min1, max1);
		GetInterval(o2, axis, min2, max2);

		float dist = distance(min1, max1, min2, max2);

		if (dist > 0)
			return 0;

		dist = abs(dist);

		if (dist < lowest)
		{
			lowest = dist;
			la = axis;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (axes[i] == axes[j + 3])
				continue;

			Vector3 axis = Vector3::Cross(axes[i], axes[j + 3]).Normalized();

			GetInterval(o1, axis, min1, max1);
			GetInterval(o2, axis, min2, max2);

			float dist = distance(min1, max1, min2, max2);

			if (dist > 0)
				return 0;

			dist = abs(dist);

			if (dist < lowest)
			{
				lowest = dist;
				la = axis;
			}
		}
	}

	if (Vector3::Dot(la, o1.parent->GetWorldPosition() - o2.parent->GetWorldPosition()) < 0)
		lowest = -lowest;

	Debug::Print("%f\n", lowest);
	*mtv = la * lowest;

	col = CollisionPoint(o1, o2, count);
	return 1;
}

//make projection of desired axis
void Collision::GetInterval(Rigidbody& o, Vector3 axis, float &min, float &max)
{
	min = max = Vector3::Dot(axis, transform(o, o.collider->GetVertex(0)));

	for (int i = 1; i < 8; i++)
	{
		float value = Vector3::Dot(axis, transform(o, o.collider->GetVertex(i)));
		min = ::min(min, value);
		max = ::max(max, value);
	}
}

Manifold cols[100];

Manifold* Collision::CollisionPoint(Rigidbody& obj1, Rigidbody& obj2, int& count)
{
	Vector3 colisionPoint1;
	Vector3 colisionPoint2;
	Vector3 collisionNormal;

	int colIndex = 0;

	//points.Clear();
	//points = List<Manifold*>();

	//vertices of object 2 in object1
	for (int i = 0; i < 8; i++)
	{
		Vector3 CP = transform(obj2, obj2.collider->GetVertex(i));
		if (isInside(CP, obj1))
		{
			//collision manifold contains position, bodies, normal and penetration depth
			Manifold colision = Manifold(CP);
			colision.Bod1 = &obj1;
			colision.Bod2 = &obj2;
			collisionNormal = getNormalFace(obj1, CP);

			//to catch cases where sides are same size and perfectly Alligned
			if (collisionNormal == Vector3(0, 0, 0))
			{
				collisionNormal = (obj2.parent->GetWorldPosition() - obj1.parent->GetWorldPosition()).Normalized();
			}

			colision.Normal = collisionNormal;
			colision.penetration = point_depth;
			cols[colIndex++] = colision;
		}

		CP = transform(obj1, obj1.collider->GetVertex(i));
		if (isInside(CP, obj2))
		{
			Manifold colision = Manifold(CP);
			colision.Bod1 = &obj2;
			colision.Bod2 = &obj1;
			collisionNormal = getNormalFace(obj2, CP);

			if (collisionNormal == Vector3(0, 0, 0))
			{
				collisionNormal = (obj1.parent->GetWorldPosition() - obj2.parent->GetWorldPosition()).Normalized();
			}

			colision.Normal = collisionNormal;
			colision.penetration = point_depth;
			cols[colIndex++] = colision;
		}
	}

	//vertices of object 1 in object 2
	//same Method as before but reversed objects.
	//for (int i = 0; i < 8; i++) {
	//Vector3 CP = obj1.getVertex(i);
	//if (isInside(CP, obj2)) {
	//Manifold * colision = new Manifold(CP);
	//colision->Bod1 = &obj2;
	//colision->Bod2 = &obj1;
	//collisionNormal = getNormalFace(obj2, CP);
	//if (collisionNormal == Vector3(0)) {
	//collisionNormal = glm::normalize(obj1.position - obj2.position);
	//}
	//colision->Normal = collisionNormal;
	//colision->penetration = point_depth;
	//points.push_back(colision);
	//}
	//}

	//edges obj1  against edges obj2
	//checks for the edges 
	for (int i = 0; i < 12; i++)
	{
		//continue;
		Vector3* pointsEd1 = obj1.collider->GetEdge(i);
		pointsEd1[0] = transform(obj1, pointsEd1[0]);
		pointsEd1[1] = transform(obj1, pointsEd1[1]);

		for (int j = 0; j < 12; j++)
		{
			Vector3* pointsEd2 = obj2.collider->GetEdge(j);
			pointsEd2[0] = transform(obj2, pointsEd2[0]);
			pointsEd2[1] = transform(obj2, pointsEd2[1]);

			closest_Point(pointsEd1[0], pointsEd1[1], pointsEd2[0], pointsEd2[1], colisionPoint1, colisionPoint2);

			int c = Debug::color;
			Debug::color = 0xFFFF0000;

			Vector3 po = (colisionPoint2 - colisionPoint1);
			//Debug::Print("%f\t%f\t%f\n", po.x, po.y, po.z);
			Debug::Print("%f\n", colisionPoint1.Magnitude() * 10000);
			//PIT::Sleep(1000);

			Debug::color = c;

			if ((colisionPoint2 - colisionPoint1).Magnitude() < 0.1f)
			{
				Vector3 CP = (colisionPoint1 + colisionPoint2) / 2.0f;
				Manifold colision = Manifold(CP);
				colision.Bod1 = &obj1;
				colision.Bod2 = &obj2;
				colision.penetration = point_depth;

				colision.Normal = getNormalEdge(pointsEd1[0], pointsEd1[1], pointsEd2[0], pointsEd2[1]);

				if (colision.penetration > DEPSILON)
					cols[colIndex++] = colision;
			}
		}
	}

	//for (int i = 0; i < points.Count(); i++)
	//{
	//	for(int j = i + 1; j )
	//}

	count = colIndex;
	return cols;
}

//Checks if point is inside the Box
bool Collision::isInside(Vector3 ip, Rigidbody obj1)
{
	Vector3 localP = transform2(obj1, ip);
	Vector3 scale = obj1.parent->GetWorldScale();

	if (localP.x < -obj1.collider->size.x * scale.x || localP.x > obj1.collider->size.x * scale.x) return 0;
	if (localP.y < -obj1.collider->size.y * scale.y || localP.y > obj1.collider->size.y * scale.y) return 0;
	if (localP.z < -obj1.collider->size.z * scale.z || localP.z > obj1.collider->size.z * scale.z) return 0;
	return 1;
}

//Method finds the closest point between 2 lines, used for edge to edge collisions
void Collision::closest_Point(Vector3 p1, Vector3 q1, Vector3 p2, Vector3 q2, Vector3&c1, Vector3&c2)
{
	Vector3 d1 = q1 - p1;  //direction of segment S1
	Vector3 d2 = q2 - p2;  //direction of segment S2

	Vector3 r = p1 - p2;
	float s, t;
	float a = Vector3::Dot(d1, d1); // Squared Length of segment S1,always positive
	float e = Vector3::Dot(d2, d2); // Squared Length of segment S2,always positive

	float f = Vector3::Dot(d2, r);
	float c = Vector3::Dot(d1, r);
	float b = Vector3::Dot(d1, d2);
	float denom = a * e - b * b;  //always positive

							  //if segments not parallel, compute closest point on L1 to L2 and clamp segment S1. Else pick arbitrary s (here 0)
	if (denom != 0.0f) {
		s = clamp((b*f - c * e) / denom, 0.0f, 1.0f);
	}
	else
		s = 0.0f;
	//Compute point on L2 closest to S1(s) using t = Dot((P1 + D1*s) - P2,D2) / Dot((D2,D2) = (b*s + f) / e
	t = (b*s + f) / e;

	//if t in [0,1] done. Else clamp t, recompute s for the new volume of t using
	//s = Dot((P2 + D2 * t) - P1,D1) / Dot(D1,D1) = (t*b-c)/a and clamp s to [0,1]
	if (t < 0.0f) {
		t = 0.0f;
		s = clamp(-c / a, 0.0f, 1.0f);
	}
	else if (t > 1.0f) {
		t = 1.0f;
		s = clamp((b - c) / a, 0.0f, 1.0f);
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;

	point_depth = (c2 - c1).Magnitude();
}

//finds the normal for collision between 2 edges
Vector3 Collision::getNormalEdge(Vector3& p1, Vector3& q1, Vector3& p2, Vector3& q2)
{
	Vector3 normal = Vector3::Cross(p1 - q1, p2 - q2).Normalized();
	if (normal.Magnitude() != 0)
		normal = normal.Normalized();
	return normal;
}

//Finds the closest face to the collision point, and takes that face's normal
Vector3 Collision::getNormalFace(Rigidbody& obj1, Vector3 ip)
{
	// Transform the point into box coordinates.
	Vector3 relPt = transform2(obj1, ip);
	Vector3 scale = obj1.parent->GetWorldScale();

	Vector3 normal;
	// Check each axis, looking for the axis on which the
	// penetration is least deep.
	float min_depth = (obj1.collider->size.x * scale.x) - abs(relPt.x);
	if (min_depth > EPSILON)
		normal = obj1.collider->GetFaceDir(0) * ((relPt.x < 0) ? -1.0f : 1.0f);

	float depth = (obj1.collider->size.y * scale.y) - abs(relPt.y);
	if (depth < min_depth && depth > EPSILON)
	{
		min_depth = depth;
		normal = obj1.collider->GetFaceDir(1) * ((relPt.y < 0) ? -1.0f : 1.0f);
	}

	depth = (obj1.collider->size.z * scale.z) - abs(relPt.z);
	if (depth < min_depth && depth > EPSILON)
	{
		min_depth = depth;
		normal = obj1.collider->GetFaceDir(2) * ((relPt.z < 0) ? -1.0f : 1.0f);
	}

	point_depth = min_depth;
	return normal;
}