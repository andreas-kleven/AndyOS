#pragma once
#include <types.h>

class TtyBuffer
{
private:
    int buf_size;
    int position;

public:
    char *buffer;
    int written;

    TtyBuffer();
    TtyBuffer(int buf_size);
    ~TtyBuffer();

    bool Write(const char *buf, size_t size);
    bool Reset();
};
