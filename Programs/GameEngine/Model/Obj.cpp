#include <stdio.h>
#include <AndyOS.h>
#include "Obj.h"

using namespace std;

Obj::Obj(char* file)
{
	ReadFile(file);
	CreateVertices();
}

void Obj::ReadFile(char* file)
{
	return;
	/*std::string text = file;
	std::vector<std::string> lines;
	text.Split(lines, '\n');

	printf("--START\n");

	for (int i = 0; i < lines.size(); i++)
	{
		std::vector<std::string> args;
		lines[i].Split(args, ' ');

		if (args[0] == "v")
		{
			float x = atof(args[1].c_str());
			float y = atof(args[2].c_str());
			float z = atof(args[3].c_str());
			positions.push_back(Vector3(x, y, z));
		}
		else if (args[0] == "vt")
		{
			float u = atof(args[1].c_str());
			float v = atof(args[2].c_str());
			uvs.push_back(Vector3(u, v, 0));
		}
		else if (args[0] == "vn")
		{
			float x = atof(args[1].c_str());
			float y = atof(args[2].c_str());
			float z = atof(args[3].c_str());
			normals.push_back(Vector3(x, y, z));
		}
		else if (args[0] == "f")
		{
			//if (args.size() == 5)
			//	Exceptions::ThrowException("Quad exception", "Obj file");

			Face face;
			for (int i = 0; i < 3; i++)
			{
				std::vector<std::string> indexes;

				args[i + 1].Split(indexes, '/');
				face.positions[i] = atoi(indexes[0].c_str());
				face.normals[i] = atoi(indexes[2].c_str());

				if (indexes[1] == "")
					face.uvs[i] = 0;
				else
					face.uvs[i] = atoi(indexes[1].c_str());

			}
			faces.push_back(face);
		}
	}

	printf("--END\n");*/
}

void Obj::CreateVertices()
{
	/*int vertex_count = faces.size() * 3;

	for (int i = 0; i < faces.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Vertex vert;

			Vector3 pos = positions[faces[i].positions[j] - 1];
			vert.pos.x = pos.x;
			vert.pos.y = pos.y;
			vert.pos.z = pos.z;

			Vector3 norm = normals[faces[i].normals[j] - 1];
			vert.normal = norm.ToVector4();

			Vector3 uv = uvs[faces[i].uvs[j] - 1];
			vert.tex_u = uv.x;
			vert.tex_v = uv.y;

			vert.color = Color(1, 1, 1);

			vertices[i * 3 + j] = vert;
		}
	}*/
}
