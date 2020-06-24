#include <stdio.h>
#include <AndyOS.h>
#include "Obj.h"
#include "A3D.h"
#include "ModelLoader.h"

Model3D* ModelLoader::LoadModel(const char* filename, Format3D format)
{
	char* buffer;

	uint32_t size = read_file(buffer, filename);

	if (size)
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
		printf("File not found '%s'\n", filename);
		while (1);
	}

	return 0;
}
