#pragma once
#include "tcp.h"

class TcpSocket
{
public:
	TcpSocket();

	bool Connect(uint32 dst, uint16 port);
	bool Listen(uint16 port);

	bool Send(uint8* data, uint32 length);

	bool Close();

private:
	TcpSession* session;
};