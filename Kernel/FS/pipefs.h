#pragma once
#include "filesystem.h"
#include "pipe.h"
#include "list.h"
#include "Drivers/driver.h"

class PipeFS : public FileSystem
{
private:
    List<Pipe> pipes;

public:
    PipeFS()
    {
        name = "pipefs";
    }

    int Mount(BlockDriver *driver, DENTRY *root_dentry);
    int Open(FILE *file);
    int Close(FILE *file);
    int Read(FILE *file, void *buf, size_t size);
    int Write(FILE *file, const void *buf, size_t size);
    int Seek(FILE *file, long offset, int origin);
    int GetChildren(DENTRY *parent, const char *find_name);

    int Create(DENTRY *&dentry, int flags);

private:
    Pipe *GetPipe(FILE *file);
};
