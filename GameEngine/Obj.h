#pragma once
#include "definitions.h"
#include "list.h"
#include "Vector3.h"
#include "Vertex.h"
#include "Model3D.h"

class Obj : public Model3D
{
public:
	std::List<Vector3> positions;
	std::List<Vector3> normals;
	std::List<Vector3> uvs;
	std::List<Face> faces;

	Obj(char* file);

private:
	void ReadFile(char* file);
	void CreateVertices();
};