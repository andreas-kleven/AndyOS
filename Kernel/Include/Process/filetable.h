#pragma once
#include <FS/file.h>
#include <list.h>

class Filetable
{
  private:
    int reserved;
    List<FILE *> files;

  public:
    Filetable(int reserved = 3);
    int Add(FILE *file, int lowest_fd = 0);
    int Remove(int fd);
    FILE *Get(int fd);
    int Set(int fd, FILE *file);
    Filetable *Clone();
};
