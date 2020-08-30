#pragma once
#include <FS/filesystem.h>
#include <driver.h>

class DevFS : public FileSystem
{
  public:
    DevFS() { name = "devfs"; }

    int Mount(BlockDriver *driver);
    int Open(FILE *file);
    int Close(FILE *file);
    int Read(FILE *file, void *buf, size_t size);
    int Write(FILE *file, const void *buf, size_t size);
    int Seek(FILE *file, long offset, int origin);
    int GetChildren(DENTRY *parent, const char *find_name);

  private:
    Driver *GetDriver(FILE *file);
};
