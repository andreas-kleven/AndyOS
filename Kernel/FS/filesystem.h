#pragma once
#include "types.h"
#include "file.h"
#include "Drivers/driver.h"

class FileSystem : public Driver
{
public:
	FileSystem()
	{
		this->type = DRIVER_TYPE_FS;
	}

	virtual int Mount(BlockDriver *driver, DENTRY *root_dentry) { return 0; }
};
