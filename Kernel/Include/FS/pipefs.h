#pragma once
#include <FS/filesystem.h>
#include <FS/pipe.h>
#include <driver.h>
#include <list.h>

class PipeFS : public FileSystem
{
  private:
    List<Pipe *> pipes;

  public:
    PipeFS() { name = "pipefs"; }

    int Mount(BlockDriver *driver);
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
