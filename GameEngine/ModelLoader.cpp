#include "ModelLoader.h"
#include "FileSystem.h"
#include "Obj.h"
#include "A3D.h"

Model3D* ModelLoader::LoadModel(char* filename, Format3D format)
{
	char* buffer;

	STATUS status = FileSystem::ReadFile(filename, buffer);

	if (status)
	{
		switch (format)
		{
		case FORMAT_OBJ:
			return new Obj(buffer);

		case FORMAT_A3D:
			return new A3D(buffer);

		default:
			break;
		}
	}
	else
	{
		Debug::Print("ERROR");
		while (1);
	}

	return 0;
}
