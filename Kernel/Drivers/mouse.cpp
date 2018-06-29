#include "mouse.h"
#include "HAL/hal.h"
#include "math.h"

int x;
int y;

int scroll_x;
int scroll_y;

bool left = 0;
bool right = 0;
bool middle = 0;

int sx;
int sy;

uint8 mouse_cycle;
uint8 mouse_byte[4];

bool initialized;

STATUS Mouse::Init(uint32 width, uint32 height, float sens)
{
	ResetPos();
	ResetScroll();

	IDT::InstallIRQ(44, (IRQ_HANDLER)Mouse_ISR);

	MouseWait(1);
	outb(MOUSE_PORT1, 0xA8);

	MouseWait(1);
	outb(MOUSE_PORT1, 0x20);

	uint8 status_byte;
	MouseWait(0);
	status_byte = (inb(MOUSE_PORT0) | 2);

	MouseWait(1);
	outb(MOUSE_PORT1, 0x60);

	MouseWait(1);
	outb(MOUSE_PORT0, status_byte);

	MouseWrite(0xF6);
	MouseRead();

	MouseWrite(0xF4);
	MouseRead();


	MouseWrite(0xF3);
	MouseRead();
	MouseWrite(200);
	MouseRead();

	MouseWrite(0xF3);
	MouseRead();
	MouseWrite(100);
	MouseRead();

	MouseWrite(0xF3);
	MouseRead();
	MouseWrite(80);
	MouseRead();

	MouseWrite(0xF2);
	MouseRead();

	initialized = 1;

	asm volatile("int $44");

	return STATUS_SUCCESS;
}

void Mouse::MouseWait(uint8 type)
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

void Mouse::MouseWrite(uint8 val)
{
	MouseWait(1);
	outb(MOUSE_PORT1, 0xD4);
	MouseWait(1);
	outb(MOUSE_PORT0, val);
}

uint8 Mouse::MouseRead()
{
	MouseWait(0);
	return inb(MOUSE_PORT0);
}

void Mouse::GetButtons(bool& _left, bool& _right, bool& _middle)
{
	_left = left;
	_right = right;
	_middle = middle;
}

void Mouse::GetPos(int& _x, int& _y)
{
	_x = x;
	_y = y;
}

void Mouse::GetScroll(int& x, int& y)
{
	x = scroll_x;
	y = scroll_y;
}

void Mouse::ResetPos()
{
	x = 0;
	y = 0;
}

void Mouse::ResetScroll()
{
	scroll_x = 0;
	scroll_y = 0;
}

void Mouse::Mouse_ISR(REGS* regs)
{
	if (initialized)
	{
		switch (mouse_cycle)
		{
		default:
			mouse_byte[0] = inb(MOUSE_PORT0);

			if (mouse_byte[0] & 8)
				mouse_cycle++;
			break;

		case 1:
			mouse_byte[1] = inb(MOUSE_PORT0);
			mouse_cycle++;
			break;

		case 2:
			mouse_byte[2] = inb(MOUSE_PORT0);

			if (mouse_byte[0] & 0x80 || mouse_byte[0] & 0x40)
			{
				//Bad packet
				break;
			}

			sx = mouse_byte[1];
 			sy = mouse_byte[2];

			//Sign
			if (mouse_byte[0] & 0x10)
				sx |= 0xFFFFFF00;

			if (mouse_byte[0] & 0x20)
				sy |= 0xFFFFFF00;

			x += sx;
			y += sy;

			left = mouse_byte[0] & 1;
			right = mouse_byte[0] >> 1 & 1;
			middle = mouse_byte[0] >> 2 & 1;

			mouse_cycle = 0;
			mouse_cycle = 3;
			break;

		case 3:
			mouse_byte[3] = inb(MOUSE_PORT0);
			mouse_cycle = 0;

			if (mouse_byte[3] & 1)
				scroll_y += mouse_byte[3];
			else
				scroll_x += mouse_byte[3];
			break;
		}
	}
}
