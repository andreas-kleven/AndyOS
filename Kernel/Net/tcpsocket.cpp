#include "tcpsocket.h"
#include "HAL/hal.h"
#include "debug.h"

TcpSocket::TcpSocket()
{
	session = TCP::CreateSession();
}

bool TcpSocket::Connect(IPv4Address dst, uint16 port)
{
	if (session->state != TCP_CLOSED)
		return 0;

	session->Connect(dst, port);
	return session->state == TCP_ESTABLISHED;
}

bool TcpSocket::Listen(uint16 port)
{
	if (session->state != TCP_CLOSED)
		return 0;

	session->Listen(port);
	return session->state == TCP_LISTEN;
}

bool TcpSocket::Send(uint8* data, uint32 length)
{
	if (session->state != TCP_ESTABLISHED)
		return 0;

	session->SendData(data, length);
	return 1;
}

bool TcpSocket::Close()
{
	session->Close();
	return session->state == TCP_CLOSED;
}
