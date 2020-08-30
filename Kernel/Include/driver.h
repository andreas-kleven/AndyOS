#pragma once
#include <FS/file.h>
#include <errno.h>
#include <types.h>

#define MKDEV(major, minor) ((((major)&0xFFFF) << 16) | ((minor)&0xFFFF))

#define MAJOR_TTY            4
#define MAJOR_CDROM          11
#define MAJOR_HARDDISK       13
#define MAJOR_INPUT          13
#define MINOR_INPUT_MOUSE    1
#define MINOR_INPUT_KEYBOARD 2

class FileSystem;

enum DRIVER_TYPE
{
    DRIVER_TYPE_BLOCK,
    DRIVER_TYPE_CHAR,
    DRIVER_TYPE_FS
};

enum DRIVER_STATUS
{
    DRIVER_STATUS_ERROR,
    DRIVER_STATUS_RUNNING,
    DRIVER_STATUS_STOPPED
};

class FileIO
{
  public:
    virtual int Open(FILE *file) { return 0; }
    virtual int Close(FILE *file) { return 0; }
    virtual int Read(FILE *file, void *buf, size_t size) { return 0; }
    virtual int Write(FILE *file, const void *buf, size_t size) { return 0; }
    virtual int Seek(FILE *file, long offset, int origin) { return 0; }
    virtual int GetChildren(DENTRY *parent, const char *find_name) { return 0; }
};

class Driver : public FileIO
{
  public:
    const char *name;
    dev_t dev;
    DRIVER_TYPE type;
    DRIVER_STATUS status;
    Driver *next;

    Driver() { this->status = DRIVER_STATUS_ERROR; }

    virtual int Ioctl(FILE *file, int request, unsigned int arg) { return -ENOTTY; }
};

class BlockDriver : public Driver
{
  public:
    BlockDriver() { this->type = DRIVER_TYPE_BLOCK; }

    virtual int Read(fpos_t pos, void *buf, size_t size) { return -1; }
    virtual int Write(fpos_t pos, const void *buf, size_t size) { return -1; }
};

class CharDriver : public Driver
{
  public:
    CharDriver() { this->type = DRIVER_TYPE_CHAR; }
};

namespace DriverManager {
void AddDriver(Driver *driver);
Driver *FirstDriver();
Driver *GetDriver(const char *name);
Driver *GetDriver(dev_t dev);
STATUS Init();
}; // namespace DriverManager
