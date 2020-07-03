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
} __attribute__((packed));

struct UDP_Packet
{
	UDP_Header* header;
	uint16 data_length;
	uint8* data;
} __attribute__((packed));

namespace UDP
{
	UdpSocket* CreateSocket(int port);
	NetPacket* CreatePacket(NetInterface* intf, IPv4Address dst, uint16 src_port, uint16 dst_port, uint8* data, uint32 data_length);

	void Send(NetInterface* intf, NetPacket* pkt);
	void HandlePacket(NetInterface* intf, IPv4_Header* ip_hdr, NetPacket* pkt);
	
	STATUS Init();
}