#pragma once
#include "udp.h"

class UdpSocket
{
public:
	void(*received_event)(IPv4_Header* ip_hdr, UDP_Packet* pkt);

	UdpSocket(uint16 port);
	~UdpSocket();

	void Send(IPv4Address dst, uint8* data, uint16 length);
	int Receive(uint8*& buffer);

private:
	NetInterface* intf;

	IPv4Address ip_address;
	uint16 src_port;
	uint16 dst_port;
};