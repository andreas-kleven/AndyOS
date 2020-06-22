#pragma once
#include "Component.h"
#include "GL.h"

class Collider
{
public:
	//
	Vector3 size;

	//Transform transform;

	Collider();

	void(*OnCollision)();

	bool IsSphere() { return bIsSphere; }
	bool IsBox() { return bIsBox; }

	Vector3 GetFaceDir(int i) 
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

	void GetEdge(int vertex, Vector3* edge1, Vector3* edge2)
	{
		switch (vertex)
		{
		case 0: *edge1 = GetVertex(0); *edge2 = GetVertex(1); break;
		case 1: *edge1 = GetVertex(1); *edge2 = GetVertex(2); break;
		case 2: *edge1 = GetVertex(2); *edge2 = GetVertex(3); break;
		case 3: *edge1 = GetVertex(3); *edge2 = GetVertex(0); break;
		case 4: *edge1 = GetVertex(4); *edge2 = GetVertex(5); break;
		case 5: *edge1 = GetVertex(5); *edge2 = GetVertex(6); break;
		case 6: *edge1 = GetVertex(6); *edge2 = GetVertex(7); break;
		case 7: *edge1 = GetVertex(7); *edge2 = GetVertex(4); break;
		case 8: *edge1 = GetVertex(7); *edge2 = GetVertex(3); break;
		case 9: *edge1 = GetVertex(6); *edge2 = GetVertex(2); break;
		case 10: *edge1 = GetVertex(5); *edge2 = GetVertex(1); break;
		case 11: *edge1 = GetVertex(4); *edge2 = GetVertex(0); break;

		default:
			//Todo: crash
			break;
		}
	}

	Vector3 GetVertex(int i)
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

		//Todo: crash
		return Vector3();
	}

protected:
	bool bIsSphere;
	bool bIsBox;
};

