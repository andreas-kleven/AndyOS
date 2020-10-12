#include <Drivers/tty.h>
#include <Process/process.h>
#include <debug.h>
#include <ioctls.h>
#include <string.h>

TtyDriver::TtyDriver(TtyBaseDriver *driver, int number)
{
    this->driver = driver;
    this->dev = MKDEV(MAJOR_TTY, number);
    this->read_pipe = new Pipe(TTY_BUFFER_SIZE);
    this->line_buffer = new TtyBuffer(TTY_BUFFER_SIZE);
    this->gid = 0;
    this->sid = 0;

    this->termios.c_lflag = ECHO | ICANON | ISIG;

    char *buf = new char[16];
    sprintf(buf, "tty%d", number);
    this->name = buf;

    driver->tty = this;

    this->status = DRIVER_STATUS_RUNNING;
}

TtyDriver::~TtyDriver()
{
    delete this->read_pipe;
    delete this->line_buffer;
    delete[] this->name;
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
    return read_pipe->Read(file, buf, size);
}

int TtyDriver::Write(FILE *file, const void *buf, size_t size)
{
    return driver->Write(buf, size);
}

int TtyDriver::Ioctl(FILE *file, int request, unsigned int arg)
{
    switch (request) {
    case TCGETS: {
        struct termios *ptr = (struct termios *)arg;
        *ptr = this->termios;
        break;
    }

    case TCSETS: {
        struct termios *ptr = (struct termios *)arg;
        this->termios = *ptr;
        kprintf("Set termios %p\n", termios.c_lflag);
        break;
    }

    case TIOCGPGRP: {
        pid_t *ptr = (pid_t *)arg;
        *ptr = gid;
        break;
    }

    case TIOCSPGRP: {
        pid_t *ptr = (pid_t *)arg;
        gid = *ptr;
        kprintf("Set tcgid %d\n", gid);
        break;
    }
    }

    return 0;
}

int TtyDriver::HandleInput(const char *input, size_t size)
{
    if (size == 1) {
        char c = input[0];

        if (termios.c_lflag & ISIG) {
            switch (c) {
            case 0x03:
                ProcessManager::HandleSignal(sid, gid, SIGINT);
                line_buffer->Reset();
                break;

            case 0x1A:
                ProcessManager::HandleSignal(sid, gid, SIGSTOP);
                line_buffer->Reset();
                break;

            case 0x1C:
                ProcessManager::HandleSignal(sid, gid, SIGQUIT);
                line_buffer->Reset();
                break;
            }
        }
    }

    if (termios.c_lflag & ICANON) {
        for (size_t i = 0; i < size; i++) {
            char c = input[i];

            if (c == '\n') {
                read_pipe->Write(0, line_buffer->buffer, line_buffer->written);
                read_pipe->Write(0, "\n", 1);
                line_buffer->Reset();
            } else {
                line_buffer->Write(&c, 1);
            }
        }
    } else {
        read_pipe->Write(0, input, size);
    }

    if (termios.c_lflag & ECHO)
        driver->Write(input, size);

    return size;
}
