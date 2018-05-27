#pragma once
#include "definitions.h"
#include "list.h"
#include "Vertex.h"
#include "Model3D.h"
#include "../GL/Vector3.h"

struct Face
{
	int positions[3];
	int normals[3];
	int uvs[3];
};

class Obj : public Model3D
{
public:
	List<Vector3> positions;
	List<Vector3> normals;
	List<Vector3> uvs;
	List<Face> faces;

	Obj(char* file);

private:
	void ReadFile(char* file);
	void CreateVertices();
};