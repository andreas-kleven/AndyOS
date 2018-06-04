#pragma once
#include "definitions.h"

#define COM_MAX_BAUD 115200

#define COM_PORT1 0x3F8
#define COM_PORT2 0x2F8
#define COM_PORT3 0x2E8
#define COM_PORT4 0x2E8

static class Serial
{
public:
	static STATUS Init(int port, int baud);
	static void Transmit(int port, char data);
	static void Transmit(int port, char* data, int length);

private:
	static bool IsTransmitEmpty(int port);
};

