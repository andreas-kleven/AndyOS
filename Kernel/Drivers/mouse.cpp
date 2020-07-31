#include <Drivers/mouse.h>
#include <hal.h>
#include <io.h>
#include <irq.h>
#include <circbuf.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define MOUSE_IRQ 12
#define MOUSE_PORT0 0x60
#define MOUSE_PORT1 0x64

#define BUFFER_SIZE 128

static bool initialized = false;
static int mouse_cycle = 0;
static char buffer[BUFFER_SIZE];
static char *mouse_bytes = buffer;
static fpos_t buffer_pos = 0;
static Event data_event = Event(false, true);

void mouse_isr()
{
	if (initialized)
	{
		switch (mouse_cycle)
		{
		default:
			mouse_bytes[0] = inb(MOUSE_PORT0);

			if (mouse_bytes[0] & 8)
				mouse_cycle = 1;
			break;

		case 1:
			mouse_bytes[1] = inb(MOUSE_PORT0);
			mouse_cycle = 2;
			break;

		case 2:
			mouse_bytes[2] = inb(MOUSE_PORT0);
			mouse_cycle = 3;
			break;

		case 3:
			mouse_bytes[3] = inb(MOUSE_PORT0);
			mouse_cycle = 0;

			buffer_pos += 4;
			mouse_bytes = &buffer[buffer_pos % BUFFER_SIZE];
			memset(mouse_bytes, 0, 4);
			data_event.Set();
			break;
		}
	}
}

static void mouse_wait(uint8 type)
{
	uint32 time = 100000;
	if (type == 0)
	{
		while (time--)
			if ((inb(MOUSE_PORT1) & 1) == 1)
				return;
		return;
	}
	else
	{
		while (time--)
			if ((inb(MOUSE_PORT1) & 2) == 0)
				return;
		return;
	}
}

static void mouse_write(uint8 val)
{
	mouse_wait(1);
	outb(MOUSE_PORT1, 0xD4);
	mouse_wait(1);
	outb(MOUSE_PORT0, val);
}

static uint8 mouse_read()
{
	mouse_wait(0);
	return inb(MOUSE_PORT0);
}

static void mouse_init()
{
	IRQ::Install(44, mouse_isr);

	mouse_wait(1);
	outb(MOUSE_PORT1, 0xA8);

	mouse_wait(1);
	outb(MOUSE_PORT1, 0x20);

	uint8 status_byte;
	mouse_wait(0);
	status_byte = (inb(MOUSE_PORT0) | 2);

	mouse_wait(1);
	outb(MOUSE_PORT1, 0x60);

	mouse_wait(1);
	outb(MOUSE_PORT0, status_byte);

	mouse_write(0xF6);
	mouse_read();

	mouse_write(0xF4);
	mouse_read();

	mouse_write(0xF3);
	mouse_read();
	mouse_write(200);
	mouse_read();

	mouse_write(0xF3);
	mouse_read();
	mouse_write(100);
	mouse_read();

	mouse_write(0xF3);
	mouse_read();
	mouse_write(80);
	mouse_read();

	mouse_write(0xF2);
	mouse_read();

	initialized = 1;

	CALL_INTERRUPT(44);
}

MouseDriver::MouseDriver()
{
	this->name = "mouse";
	this->status = DRIVER_STATUS_RUNNING;

	mouse_init();
}

int MouseDriver::Open(FILE *file)
{
	file->pos = buffer_pos;
	return 0;
}

int MouseDriver::Read(FILE *file, void *buf, size_t size)
{
	while (file->pos >= buffer_pos)
	{
		if (file->flags & O_NONBLOCK)
			return -EAGAIN;

		data_event.Wait();
	}

	if (size > BUFFER_SIZE)
		size = BUFFER_SIZE;

	int read = 0;
	fpos_t pos = file->pos;
	char *ptr = (char *)buf;

	// TODO: memcpy
	while (read < size && pos < buffer_pos)
	{
		ptr[read] = buffer[pos % BUFFER_SIZE];
		read += 1;
		pos += 1;
	}

	return read;
}
