#include <FS/pipe.h>
#include <string.h>

Pipe::Pipe(int buf_size)
{
    read_event = Event();
    write_event = Event(true);
    buffer_mutex = Mutex();
    buffer = new CircularDataBuffer(buf_size);
}

Pipe::~Pipe()
{
    delete buffer;
}

int Pipe::Read(FILE *file, void *buf, size_t size)
{
    if (!read_event.WaitIntr())
        return -EINTR;

    buffer_mutex.Aquire();

    int ret = buffer->Read(size, buf);

    if (buffer->IsEmpty())
        read_event.Clear();

    if (!buffer->IsFull())
        write_event.Set();

    buffer_mutex.Release();
    return ret;
}

int Pipe::Write(FILE *file, const void *buf, size_t size)
{
    int written = 0;

    while (written < (int)size) {
        if (!write_event.WaitIntr())
            return written ? written : -EINTR;

        buffer_mutex.Aquire();

        written += buffer->Write(buf, size);

        if (!buffer->IsEmpty())
            read_event.Set();

        if (buffer->IsFull())
            write_event.Clear();

        buffer_mutex.Release();
    }

    return written;
}

int Pipe::Close(FILE *file)
{
    read_event.Set();
    return 0;
}
