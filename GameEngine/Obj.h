#pragma once
#include "definitions.h"
#include "list.h"
#include "Vector.h"
#include "Vertex.h"
#include "Model3D.h"

class Obj : public Model3D
{
public:
	std::List<Vector> positions;
	std::List<Vector> normals;
	std::List<Vector> uvs;
	std::List<Face> faces;

	Obj(char* file);

private:
	void ReadFile(char* file);
	void CreateVertices();
};