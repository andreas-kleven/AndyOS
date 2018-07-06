#pragma once
#include "definitions.h"
#include "Drivers/driver.h"
#include "vfs.h"

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

class FileSystem : public Driver
{
public:
	char* name;
	char* mount_point;

	FileSystem();

	virtual int GetFile(const char* path, FNODE*) { return -1; }
};