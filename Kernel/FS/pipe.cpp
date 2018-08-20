#include "pipe.h"
#include "string.h"

Pipe::Pipe(int buf_size)
{
    buffer = CircularDataBuffer(buf_size);
}

int Pipe::Read(FILE* file, char* buf, size_t size)
{
    return buffer.Read(size, buf);
}

int Pipe::Write(FILE* file, const char* buf, size_t size)
{
    return buffer.Write(buf, size);
}

int Pipe::Seek(FILE* file, long offset, int origin)
{
    return -1;
}