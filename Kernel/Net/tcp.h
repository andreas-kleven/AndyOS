#pragma once
#include "tcptypes.h"
#include "ipv4.h"
#include "tcpsession.h"

#define MAX_SESSIONS 32

class TCP
{
public:
	static TcpSession* sessions[MAX_SESSIONS];

	static void Send(NetInterface* intf, NetPacket* pkt);
	static NetPacket* CreatePacket(NetInterface* intf, IPv4Address dst, uint16 src_port, uint16 dst_port, uint8 flags, uint32 seq, uint32 ack, uint8* data, uint32 data_length);

	static void Receive(NetInterface* intf, IPv4_Header* ip_hdr, NetPacket* pkt);
	static bool Decode(TCP_Packet* tp, NetPacket* pkt);

	static TcpSession* CreateSession();
};
