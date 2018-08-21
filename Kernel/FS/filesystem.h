#pragma once
#include "types.h"
#include "Drivers/driver.h"
#include "vfs.h"

class FileSystem : public Driver
{
public:
	char* name;
	char* mount_point;

	FileSystem();
	
	virtual int Mount(FNODE* node) { return ERROR; }
	virtual int GetFile(const Path* path, FNODE*) { return ERROR; }
};