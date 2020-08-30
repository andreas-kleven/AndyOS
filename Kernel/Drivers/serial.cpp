#include <Drivers/serial.h>
#include <hal.h>
#include <debug.h>

#define COM_REG_DATA			0
#define COM_REG_INTERRUPT		1
#define COM_REG_INTERRUPT_ID	2
#define COM_REG_LINE_CONTROL	3
#define COM_REG_MODEM_CONTROL	4
#define COM_REG_LINE_STATUS		5
#define COM_REG_MODEL_STATUS	6
#define COM_REG_MODEL_SCRATCH	7

namespace Serial
{
	bool IsTransmitEmpty(int port)
	{
		return inb(port + COM_REG_LINE_STATUS) & 0x20;
	}

	bool SerialReceived(int port)
	{
		return inb(port + COM_REG_LINE_STATUS) & 1;
	}

	void Transmit(int port, char data)
	{
		while (!IsTransmitEmpty(port));
		outb(port, data);
	}

	void Transmit(int port, char* data, int length)
	{
		while (length--)
			Transmit(port, *data++);
	}

	char Receive(int port)
	{
		while (SerialReceived(port) == 0);
		return inb(port);
	}

	STATUS Init(int port, int baud)
	{
		if (baud > COM_MAX_BAUD)
			return STATUS_FAILED;

		if (COM_MAX_BAUD % baud != 0)
			return STATUS_FAILED;

		uint16 divisor = COM_MAX_BAUD / baud;
		uint8 low = divisor & 0xFF;
		uint8 high = divisor >> 8;

		outb(port + COM_REG_INTERRUPT, 0x00);		// Disable all interrupts
		outb(port + COM_REG_LINE_CONTROL, 0x80);	// Enable DLAB (set baud rate divisor)
		outb(port + COM_REG_DATA, low);
		outb(port + COM_REG_INTERRUPT, high);
		outb(port + COM_REG_LINE_CONTROL, 0x03);	// 8 bits, no parity, one stop bit
		outb(port + COM_REG_INTERRUPT_ID, 0xC7);	// Enable FIFO, clear them, with 14-byte threshold
		outb(port + COM_REG_MODEM_CONTROL, 0x0B);	// IRQs enabled, RTS/DSR set

		return STATUS_SUCCESS;
	}
}
