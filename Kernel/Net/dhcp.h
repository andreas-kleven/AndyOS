#pragma once
#include "udp.h"

#define PORT_DHCP_SRC	67
#define PORT_DHCP_DST	68

struct DHCP_Header
{
	uint8 op;
	uint8 htype;
	uint8 hlen;
	uint8 hops;

	uint32 xid;

	uint16 secs;
	uint16 flags;

	IPv4Address ciaddr;
	IPv4Address yiaddr;
	IPv4Address siaddr;
	IPv4Address giaddr;
	MacAddress chaddr;

	char pad[10];

	char sname[64];
	char file[128];

	uint32 magic;

	uint8 options[308];
} __attribute__((packed));

namespace DHCP
{
	void Discover(NetInterface* intf);
	void Receive(NetInterface* intf, IPv4_Header* ip_hdr, UDP_Packet* udp, NetPacket* pkt);
}