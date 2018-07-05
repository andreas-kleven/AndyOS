#pragma once
#include "tcp.h"
#include "string.h"

class HttpRequest
{
public:

};

class WebClient
{
public:
	WebClient();

	//void Send(NetInterface* intf, IPv4Address dst, uint16 port, HttpRequest req);
	//void Send(String addr, uint8* data, uint32 length);
	//int Download(String addr, uint8*& data);

private:
	TcpSession* session;

};