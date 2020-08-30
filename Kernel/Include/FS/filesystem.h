#pragma once
#include <FS/file.h>
#include <driver.h>
#include <types.h>

class FileSystem : public Driver
{
  public:
    DENTRY *root_dentry;

    FileSystem() { this->type = DRIVER_TYPE_FS; }

    virtual int Mount(BlockDriver *driver) { return 0; }
};
