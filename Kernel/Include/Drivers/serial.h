#pragma once
#include <types.h>

#define COM_MAX_BAUD 115200

#define COM_PORT1 0x3F8
#define COM_PORT2 0x2F8
#define COM_PORT3 0x2E8
#define COM_PORT4 0x2E8

namespace Serial
{
	void Transmit(int port, char data);
	void Transmit(int port, char* data, int length);
	char Receive(int port);
	STATUS Init(int port, int baud);
};

