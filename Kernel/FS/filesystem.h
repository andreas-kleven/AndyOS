#pragma once
#include "types.h"
#include "file.h"
#include "Drivers/driver.h"

class FileSystem : public Driver
{
public:
	DENTRY *root_dentry;

	FileSystem()
	{
		this->type = DRIVER_TYPE_FS;
	}

	virtual int Mount(BlockDriver *driver) { return 0; }
};
