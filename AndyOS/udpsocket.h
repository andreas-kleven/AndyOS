#pragma once
#include "udp.h"

class UdpSocket
{
public:
	void(*received_event)(IPv4_Header* ip_hdr, UDP_Packet* pkt);

	UdpSocket(uint16 src_port, uint16 dst_port);
	~UdpSocket();

	void Send(IPv4Address dst, uint8* data, uint16 length);
	int Receive(uint8*& buffer, IPv4Address& addr);

	void SetReceivedData(IPv4Address addr, uint8* buffer, uint16 length);

private:
	NetInterface* intf;

	uint16 src_port;
	uint16 dst_port;

	IPv4Address pkt_addr;
	uint8* pkt_buffer;
	uint16 pkt_length;
};