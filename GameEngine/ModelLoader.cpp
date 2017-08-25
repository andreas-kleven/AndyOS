#include "ModelLoader.h"
#include "FileSystem.h"
#include "Obj.h"
#include "A3D.h"

char* ModelLoader::buffer;

Model3D* ModelLoader::LoadModel(char* filename, Format3D format)
{
	FileSystem::ReadFile(filename, buffer);

	switch (format)
	{
	case FORMAT_OBJ:
		return new Obj(buffer);

	case FORMAT_A3D:
		return new A3D(buffer);

	default:
		break;
	}

	return 0;
}
