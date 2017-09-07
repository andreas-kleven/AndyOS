#pragma once
#include "Component.h"

class Collider
{
public:
	//
	Vector3 size;

	Transform transform;

	Collider();

	void(*OnCollision)();

	inline bool IsSphere() { return bIsSphere; }
	inline bool IsBox() { return bIsBox; }

	virtual Vector3 GetFaceDir(int i) 
	{
		Vector3 direction;
		switch (i) {
			//right face
		case 0:
			direction = Vector3(size.x / 2, 0, 0);
			direction = direction.Normalized();
			return direction; break;
			//top face
		case 1:
			direction = Vector3(0, size.y / 2, 0);
			direction = direction.Normalized();
			return direction; break;
			//back face
		case 2:
			direction = Vector3(0, 0, size.z / 2);
			direction = direction.Normalized();
			return direction; break;
		}
		return direction;
	}

	virtual Vector3 GetEdgeDir(int i) 
	{ 
		Vector3 direction;
		switch (i) {
			//front top corner
		case 0:
			direction = Vector3(0, size.y / 2, size.z / 2);
			direction = direction.Normalized();
			return direction; break;
			// right top corner
		case 1:
			direction = Vector3(size.x / 2, size.y / 2, 0);
			direction = direction.Normalized();
			return direction; break;
			//front right corner
		case 2:
			direction = Vector3(size.x / 2, 0, size.z / 2);
			direction = direction.Normalized();
			return direction; break;
		}
		return direction;
	}

	virtual Vector3* GetEdge(int vertex) 
	{
		Vector3 edges[2];

		switch (vertex) {
		case 0: edges[0] = GetVertex(0); edges[1] = GetVertex(1); break;
		case 1: edges[0] = GetVertex(1); edges[1] = GetVertex(2); break;
		case 2: edges[0] = GetVertex(2); edges[1] = GetVertex(3); break;
		case 3: edges[0] = GetVertex(3); edges[1] = GetVertex(0); break;
		case 4: edges[0] = GetVertex(4); edges[1] = GetVertex(5); break;
		case 5: edges[0] = GetVertex(5); edges[1] = GetVertex(6); break;
		case 6: edges[0] = GetVertex(6); edges[1] = GetVertex(7); break;
		case 7: edges[0] = GetVertex(7); edges[1] = GetVertex(4); break;
		case 8: edges[0] = GetVertex(7); edges[1] = GetVertex(3); break;
		case 9: edges[0] = GetVertex(6); edges[1] = GetVertex(2); break;
		case 10: edges[0] = GetVertex(5); edges[1] = GetVertex(1); break;
		case 11: edges[0] = GetVertex(4); edges[1] = GetVertex(0); break;
		}

		return edges;
	}

	virtual Vector3 GetVertex(int i)
	{
		Vector3 vertex;

		switch (i) {
		case 0: vertex = Vector3(size.x*0.5f, size.y*0.5f, size.z*0.5f); break;//righttopfront
		case 1: vertex = Vector3(size.x*0.5f, size.y*0.5f, -size.z*0.5f); break;//righttopback
		case 2: vertex = Vector3(-size.x*0.5f, size.y*0.5f, -size.z*0.5f); break;//lefttopback
		case 3: vertex = Vector3(-size.x*0.5f, size.y*0.5f, size.z*0.5f); break;//lefttopfront
		case 4: vertex = Vector3(size.x*0.5f, -size.y*0.5f, size.z*0.5f); break;//rightbottomfront
		case 5: vertex = Vector3(size.x*0.5f, -size.y*0.5f, -size.z*0.5f); break;//rightbottomback
		case 6: vertex = Vector3(-size.x*0.5f, -size.y*0.5f, -size.z*0.5f); break;//leftbottomback
		case 7: vertex = Vector3(-size.x*0.5f, -size.y*0.5f, size.z*0.5f); break;//leftbottomfront
		}
		return vertex;
	}

protected:
	bool bIsSphere;
	bool bIsBox;
};

