#pragma once
#include "filesystem.h"
#include "Drivers/driver.h"

class DevFS : public FileSystem
{
public:
    int Mount(BlockDriver *driver, DENTRY *root_dentry);
    int Open(FILE *file);
    int Close(FILE *file);
    int Read(FILE *file, void *buf, size_t size);
    int Write(FILE *file, const void *buf, size_t size);
    int Seek(FILE *file, long offset, int origin);
    int GetChildren(DENTRY *parent, const char *find_name);

private:
    Driver* GetDriver(FILE *file);
};
