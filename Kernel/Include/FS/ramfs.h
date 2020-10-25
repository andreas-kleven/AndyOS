#pragma once
#include <FS/filesystem.h>
#include <driver.h>

#define RAMFS_CHUNK_SIZE 0x1000

struct RAMFS_CHUNK
{
    RAMFS_CHUNK *next = 0;
    size_t capacity = 0;
    size_t length = 0;
    void *data = 0;
};

struct RAMFS_ENTRY
{
    char *name = 0;
    INODE inode;
    RAMFS_CHUNK *first_chunk = 0;
    RAMFS_CHUNK *last_chunk = 0;
    List<RAMFS_ENTRY *> children;
};

class RamFS : public FileSystem
{
  public:
    RamFS() { name = "ramfs"; }

    int Mount(BlockDriver *driver);
    int Open(FILE *file);
    int Close(FILE *file);
    int Read(FILE *file, void *buf, size_t size);
    int Write(FILE *file, const void *buf, size_t size);
    int Seek(FILE *file, long offset, int origin);
    int Create(DENTRY *parent, const char *name, mode_t mode);
    int GetChildren(DENTRY *parent, const char *find_name);

  private:
    ino_t next_inode;

    RAMFS_CHUNK *SeekChunk(RAMFS_ENTRY *entry, FILE *file, size_t *offset);
};
