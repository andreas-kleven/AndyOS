#include <Drivers/tty.h>
#include <debug.h>

TtyDriver::TtyDriver(TtyBaseDriver *driver, int number)
{
    this->driver = driver;
    this->dev = MKDEV(MAJOR_TTY, number);
    this->read_pipe = Pipe(TTY_BUFFER_SIZE);
    this->line_buffer = TtyBuffer(TTY_BUFFER_SIZE);

    char *buf = new char[16];
    sprintf(buf, "tty%d", number);
    this->name = buf;

    driver->tty = this;

    this->status = DRIVER_STATUS_RUNNING;
}

int TtyDriver::Open(FILE *file)
{
    return driver->Open();
}

int TtyDriver::Close(FILE *file)
{
    return driver->Close();
}

int TtyDriver::Read(FILE *file, void *buf, size_t size)
{
    return read_pipe.Read(file, buf, size);
}

int TtyDriver::Write(FILE *file, const void *buf, size_t size)
{
    return driver->Write(buf, size);
}

int TtyDriver::Ioctl(FILE *file, int request, unsigned int arg)
{
    return 0;
}

int TtyDriver::HandleInput(const char *input, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        char c = input[i];

        if (c == '\n')
        {
            debug_dump(line_buffer.buffer, line_buffer.written);
            read_pipe.Write(0, line_buffer.buffer, line_buffer.written);
            read_pipe.Write(0, "\n", 1);
            line_buffer.Reset();
        }
        else
        {
            line_buffer.Write(&c, 1);
        }
    }

    if (true)
        driver->Write(input, size);

    return size;
}
