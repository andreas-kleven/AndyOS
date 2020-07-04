#pragma once
#include "ipv4.h"
#include "tcptypes.h"

struct TcpLinkPacket
{
	TCP_Packet* pkt;
	TcpLinkPacket* next;
	TcpLinkPacket* prev;
} __attribute__((packed));

class TcpSession
{
public:
	int state;

	uint32 dst_ip;
	uint16 dst_port;
	uint16 src_port;

	TcpSession();
	void Receive(IPv4_Header* ip_hdr, TCP_Packet* tcp);

	void Connect(uint32 dst, uint16 port);
	void Listen(uint16 port);
	void Close();
	
	bool SendData(uint8* data, uint32 data_length);
	void ReceivedData(IPv4_Header* ip_hdr, TCP_Packet* tcp);

private:
	uint8 flags;
	int next_seq;
	int next_ack;

	bool Send(uint8 flags, uint8* data = 0, uint32 data_length = 0);
	void Reset();
};