#pragma once
#include <FS/pipe.h>
#include <circbuf.h>
#include <driver.h>
#include <termios.h>
#include <ttybuf.h>
#include <types.h>

#define TTY_BUFFER_SIZE 4096

class TtyBaseDriver;

class TtyDriver : public CharDriver
{
  private:
    TtyBaseDriver *driver;
    Pipe *read_pipe;
    TtyBuffer *line_buffer;
    int line_buffer_pos;

  public:
    gid_t gid;
    pid_t sid;
    struct termios termios;

    TtyDriver(TtyBaseDriver *driver, int number);
    ~TtyDriver();

    int Open(FILE *file);
    int Close(FILE *file);
    int Read(FILE *file, void *buf, size_t size);
    int Write(FILE *file, const void *buf, size_t size);
    int Ioctl(FILE *file, int request, unsigned int arg);

    int HandleInput(const char *input, size_t size);
};

class TtyBaseDriver
{
  public:
    TtyDriver *tty = 0;

    virtual int Open() { return -ENOSYS; }
    virtual int Close() { return -ENOSYS; }
    virtual int Write(const void *buf, size_t size) { return -ENOSYS; }
};
