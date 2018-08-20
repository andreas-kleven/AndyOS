#include "pipe.h"
#include "string.h"

Pipe::Pipe(int buf_size)
{
    this->buf_size = buf_size;
    this->buffer = new char[buf_size];
    this->empty = false;
    this->head = 0;
    this->tail = 0;
}

int Pipe::Read(FILE* file, char* buf, size_t size)
{
    strncpy(buf, "Pipe data", size);
    return 0;
    return -1;
}

int Pipe::Write(FILE* file, const char* buf, size_t size)
{
    return -1;
}

int Pipe::Seek(FILE* file, long offset, int origin)
{
    return -1;
}