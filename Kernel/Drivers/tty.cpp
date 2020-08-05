#include <Drivers/tty.h>
#include <Process/process.h>
#include <debug.h>

TtyDriver::TtyDriver(TtyBaseDriver *driver, int number)
{
    this->driver = driver;
    this->dev = MKDEV(MAJOR_TTY, number);
    this->read_pipe = Pipe(TTY_BUFFER_SIZE);
    this->line_buffer = TtyBuffer(TTY_BUFFER_SIZE);
    this->gid = 0;

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
    if (request == 0x540F)
    {
        pid_t *ptr = (pid_t *)arg;
        *ptr = gid;
    }
    else if (request == 0x5410)
    {
        pid_t *ptr = (pid_t *)arg;
        gid = *ptr;
        debug_print("Set tcgid %d\n", gid);
    }

    return 0;
}

int TtyDriver::HandleInput(const char *input, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        char c = input[i];

        switch (c)
        {
        case 0x03:
            ProcessManager::HandleSignal(0, gid, SIGINT);
            line_buffer.Reset();
            break;

        case 0x1A:
            ProcessManager::HandleSignal(0, gid, SIGSTOP);
            line_buffer.Reset();
            break;

        case 0x1C:
            ProcessManager::HandleSignal(0, gid, SIGQUIT);
            line_buffer.Reset();
            break;

        default:
            if (c == '\n')
            {
                read_pipe.Write(0, line_buffer.buffer, line_buffer.written);
                read_pipe.Write(0, "\n", 1);
                line_buffer.Reset();
            }
            else
            {
                line_buffer.Write(&c, 1);
            }

            break;
        }
    }

    if (true)
        driver->Write(input, size);

    return size;
}
