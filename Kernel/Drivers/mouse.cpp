#include "mouse.h"
#include "HAL/hal.h"
#include "circbuf.h"
#include "math.h"
#include "string.h"

#define MOUSE_IRQ 12
#define MOUSE_PORT0 0x60
#define MOUSE_PORT1 0x64

#define BUFFER_SIZE 128

static bool initialized;
static int mouse_cycle = 0;
static char* mouse_byte;
static size_t buffer_pos;
static char buffer[BUFFER_SIZE];

void mouse_isr()
{
	if (initialized)
	{
		switch (mouse_cycle)
		{
		default:
			mouse_byte = &buffer[(buffer_pos + 4) % BUFFER_SIZE];
			
			memset(mouse_byte, 0, 4);
			buffer_pos += 4;

			mouse_byte[0] = inb(MOUSE_PORT0);

			if (mouse_byte[0] & 8)
				mouse_cycle = 1;
			break;

		case 1:
			mouse_byte[1] = inb(MOUSE_PORT0);
			mouse_cycle = 2;
			break;

		case 2:
			mouse_byte[2] = inb(MOUSE_PORT0);
			mouse_cycle = 3;
			break;

		case 3:
			mouse_byte[3] = inb(MOUSE_PORT0);
			mouse_cycle = 0;
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
	IDT::InstallIRQ(44, (IRQ_HANDLER)mouse_isr);

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

	asm volatile("int $44");
}

MouseDriver::MouseDriver()
{
	this->id = "mouse";
	this->status = DRIVER_STATUS_RUNNING;

	mouse_init();
}

int MouseDriver::Open(FNODE* node, FILE* file)
{
	file->pos = buffer_pos;
	return SUCCESS;
}

int MouseDriver::Read(FILE* file, char* buf, size_t size)
{
	if (file->pos >= buffer_pos)
		return 0;

	if (size > BUFFER_SIZE)
		size = BUFFER_SIZE;

	int index = file->pos % BUFFER_SIZE;
	memcpy(buf, &buffer[index], size);

	return size;
}