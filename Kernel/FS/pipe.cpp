#include "pipe.h"
#include "string.h"

Pipe::Pipe(int buf_size)
{
    buffer = CircularDataBuffer(buf_size);
    write_event.Set();
}

int Pipe::Read(FILE *file, void *buf, size_t size)
{
    read_event.Wait();
    int ret = buffer.Read(size, buf);

    if (buffer.IsEmpty())
        read_event.Clear();

    if (!buffer.IsFull())
        write_event.Set();

    return ret;
}

int Pipe::Write(FILE *file, const void *buf, size_t size)
{
    write_event.Wait();
    read_event.Set();
    int ret = buffer.Write(buf, size);

    if (!buffer.IsEmpty())
        read_event.Set();

    if (buffer.IsFull())
        write_event.Clear();

    return ret;
}

int Pipe::Close(FILE *file)
{
    read_event.Set();
    return 0;
}
