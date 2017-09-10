#pragma once
#include "Component.h"

class Collider
{
public:
	//
	Vector3 size;

	//Transform transform;

	Collider();

	void(*OnCollision)();

	inline bool IsSphere() { return bIsSphere; }
	inline bool IsBox() { return bIsBox; }

	virtual Vector3 GetFaceDir(int i) 
	{
		switch (i) 
		{
		case 0: //right
			return Vector3(1, 0, 0);

		case 1: // top
			return Vector3(0, 1, 1);

		case 2: //back
			return Vector3(0, 0, 1);

		case 3:
			return Vector3(-1, 0, 0);

		case 4:
			return Vector3(0, -1, 0);

		case 5:
			return Vector3(0, 0, -1);

		default:
			return Vector3();
		}
	}

	virtual Vector3 GetEdgeDir(int i) 
	{ 
		/*if (i < 4)
		{
			return Vector3(1, 0, 0);
		}
		else if (i < 8)
		{
			return Vector3(0, 1, 0);
		}
		else
		{
			return Vector3(0, 0, 1);
		}*/

		switch (i) 
		{
		case 0:
			return Vector3(1, 0, 0);

		case 1:
			return Vector3(0, 1, 0);

		case 2:
			return Vector3(0, 0, 1);
		}
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
		switch (i) 
		{
		case 0: return Vector3(size.x, size.y, size.z);
		case 1: return Vector3(size.x, size.y, -size.z);
		case 2: return Vector3(size.x, -size.y, size.z);
		case 3: return Vector3(size.x, -size.y, -size.z);
		case 4: return Vector3(-size.x, size.y, size.z);
		case 5: return Vector3(-size.x, size.y, -size.z);
		case 6: return Vector3(-size.x, -size.y, size.z);
		case 7: return Vector3(-size.x, -size.y, -size.z);
		}
	}

protected:
	bool bIsSphere;
	bool bIsBox;
};

