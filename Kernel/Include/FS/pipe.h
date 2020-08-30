#pragma once
#include <circbuf.h>
#include <driver.h>
#include <types.h>

#define PIPE_BUF_SIZE 4096

class Pipe
{
  private:
    Event read_event;
    Event write_event;
    Mutex buffer_mutex;
    CircularDataBuffer *buffer;

  public:
    Pipe(int buf_size = PIPE_BUF_SIZE);
    ~Pipe();

    int Close(FILE *file);
    int Read(FILE *file, void *buf, size_t size);
    int Write(FILE *file, const void *buf, size_t size);
};
