#include <string.h>
#include <ttybuf.h>

TtyBuffer::TtyBuffer()
{}

TtyBuffer::TtyBuffer(int buf_size)
{
    this->buf_size = buf_size;
    this->buffer = new char[buf_size];
    Reset();
}

TtyBuffer::~TtyBuffer()
{
    delete[] this->buffer;
}

bool TtyBuffer::Write(const char *buf, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        char c = buf[i];

        if (c == '\b') {
            if (position > 0) {
                int len = written - position;
                memcpy(&buffer[position - 1], &buffer[position], len);
                position -= 1;
                written -= 1;
            }
        } else {
            if (position >= buf_size - 1)
                return false;

            buffer[position] = c;
            position += 1;
            written += 1;
        }
    }

    return true;
}

bool TtyBuffer::Reset()
{
    written = 0;
    position = 0;
    return true;
}
