#pragma once
#include "definitions.h"
#include "../Drivers/device.h"

struct DIRECTORY_INFO
{
	char* name;
	char* path;
	uint32 attributes;
};

struct FILE_INFO
{
	char* name;
	char* path;
	uint32 location;
	uint32 size;
	uint32 attributes;
};

class IFileSystem
{
public:
	BlockDevice * device;
	IFileSystem* next;

	IFileSystem(BlockDevice* dev);

	virtual bool GetDirectory(DIRECTORY_INFO* parent, char* path, DIRECTORY_INFO* dir) { return 0; }
	virtual bool GetFile(DIRECTORY_INFO* dir, char* path, FILE_INFO* file) { return 0; }
	virtual bool ReadFile(FILE_INFO* file, char*& buffer) { return 0; }
	virtual bool WriteFile(FILE_INFO* file, void* data, uint32 length) { return 0; }
};