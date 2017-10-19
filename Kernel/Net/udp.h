#pragma once
#include "ipv4.h"

class UdpSocket;

#define UDP_MAX_PORTS		65536
#define UDP_MAX_PACKET_SIZE 65536

struct UDP_Header
{
	uint16 src_port;
	uint16 dst_port;
	uint16 length;
	uint16 checksum;
};

struct UDP_Packet
{
	UDP_Header* header;
	uint16 data_length;
	uint8* data;
};

static class UDP
{
public:
	static STATUS Init();
	static UdpSocket* CreateSocket(int port);

	static void Send(NetInterface* intf, NetPacket* pkt);
	static NetPacket* CreatePacket(NetInterface* intf, IPv4Address dst, uint16 src_port, uint16 dst_port, uint8* data, uint32 data_length);
	static void Receive(NetInterface* intf, IPv4_Header* ip_hdr, NetPacket* pkt);
	static bool Decode(UDP_Packet* up, NetPacket* pkt);
	//static void SendPacket(NetInterface* intf, UDP_Header* header, IPv4Address tip);
};