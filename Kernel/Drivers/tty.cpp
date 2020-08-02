#include <Drivers/tty.h>
#include <debug.h>

TtyDriver::TtyDriver(CharDriver *driver, int number)
{
    this->driver = driver;
    this->dev = MKDEV(MAJOR_TTY, number);

    char *buf = new char[16];
    sprintf(buf, "tty%d", number);
    this->name = buf;

    this->status = DRIVER_STATUS_RUNNING;
}

int TtyDriver::Open(FILE *file)
{
    return driver->Open(file);
}

int TtyDriver::Close(FILE *file)
{
    return driver->Close(file);
}

int TtyDriver::Read(FILE *file, void *buf, size_t size)
{
    char *ptr = (char *)buf;
    int read = 0;

    while (read < size)
    {
        int ret = driver->Read(file, ptr, 1);

        if (ret < 0)
            return ret;

        if (ret == 0)
            return read;

        char c = *ptr;

        if (c == '\b')
        {
            if (read > 0)
            {
                read -= 1;
                ptr -= 1;
            }
        }
        else
        {
            read += ret;
            ptr += ret;
        }

        if (c == '\n')
            return read;
    }

    return read;
}

int TtyDriver::Write(FILE *file, const void *buf, size_t size)
{
    return driver->Write(file, buf, size);
}
