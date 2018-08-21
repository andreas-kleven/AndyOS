#include "pipe.h"
#include "string.h"

Event read_event;
Event write_event;

Pipe::Pipe(int buf_size)
{
    buffer = CircularDataBuffer(buf_size);
    write_event.Set();
}

int Pipe::Read(FILE* file, char* buf, size_t size)
{
    read_event.Wait();
    int ret = buffer.Read(size, buf);

    if (buffer.IsEmpty())
        read_event.Clear();

    if (!buffer.IsFull())
        write_event.Set();

    return ret;
}

int Pipe::Write(FILE* file, const char* buf, size_t size)
{
    read_event.Set();
    int ret = buffer.Write(buf, size);

    if (!buffer.IsEmpty())
        read_event.Set();

    if (buffer.IsFull())
        write_event.Clear();

    return ret;
}

int Pipe::Seek(FILE* file, long offset, int origin)
{
    return -1;
}