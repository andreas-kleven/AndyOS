#pragma once
#include "types.h"
#include "file.h"
#include "Drivers/driver.h"

class FileSystem : public Driver
{
public:
	virtual int Mount(BlockDriver *driver, DENTRY *root_dentry) { return 0; }
};
