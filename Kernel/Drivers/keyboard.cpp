#include <Drivers/keyboard.h>
#include <Drivers/vtty.h>
#include <circbuf.h>
#include <debug.h>
#include <errno.h>
#include <fcntl.h>
#include <hal.h>
#include <io.h>
#include <irq.h>
#include <string.h>

#define KEYBOARD_IRQ 33
#define BUFFER_SIZE  128

static uint8 buffer[BUFFER_SIZE];
static fpos_t buffer_pos = 0;
static Event data_event;

void Keyboard_ISR()
{
    if (inb(0x64) & 1) {
        uint8 scan = inb(0x60);
        VTTY::QueueInput(scan);
        buffer[buffer_pos % BUFFER_SIZE] = scan;
        buffer_pos += 1;
        data_event.Set();
    }
}

KeyboardDriver::KeyboardDriver()
{
    data_event = Event(false, true);

    this->name = "keyboard";
    this->dev = MKDEV(MAJOR_INPUT, MINOR_INPUT_KEYBOARD);
    this->status = DRIVER_STATUS_RUNNING;

    IRQ::Install(KEYBOARD_IRQ, Keyboard_ISR);
}

int KeyboardDriver::Open(FILE *file)
{
    file->pos = buffer_pos;
    return 0;
}

int KeyboardDriver::Read(FILE *file, void *buf, size_t size)
{
    while (file->pos >= buffer_pos) {
        if (file->flags & O_NONBLOCK)
            return -EAGAIN;

        if (!data_event.WaitIntr())
            return -EINTR;
    }

    if (size > BUFFER_SIZE)
        size = BUFFER_SIZE;

    int read = 0;
    fpos_t pos = file->pos;
    char *ptr = (char *)buf;

    // TODO: memcpy
    while (read < (int)size && pos < buffer_pos) {
        ptr[read] = buffer[pos % BUFFER_SIZE];
        read += 1;
        pos += 1;
    }

    return read;
}
