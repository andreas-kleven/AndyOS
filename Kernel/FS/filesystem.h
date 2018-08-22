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
	
	virtual bool Mount(FNODE* node) { return false; }
	virtual bool GetFile(const Path& path, FNODE*) { return false; }
};