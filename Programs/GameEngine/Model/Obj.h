#pragma once
#include <vector>
#include <sys/types.h>
#include <AndyOS.h>
#include "Model3D.h"
#include "GL.h"

struct Face
{
	int positions[3];
	int normals[3];
	int uvs[3];
};

class Obj : public Model3D
{
public:
	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector3> uvs;
	std::vector<Face> faces;

	Obj(char* file);

private:
	void ReadFile(char* file);
	void CreateVertices();
};