#pragma once
#include <Net/udp.h>

#define PORT_DHCP_SRC 67
#define PORT_DHCP_DST 68

struct DHCP_Header
{
	uint8 op;
	uint8 htype;
	uint8 hlen;
	uint8 hops;

	uint32 xid;

	uint16 secs;
	uint16 flags;

	uint32 ciaddr;
	uint32 yiaddr;
	uint32 siaddr;
	uint32 giaddr;
	MacAddress chaddr;

	char pad[10];

	char sname[64];
	char file[128];

	uint32 magic;

	uint8 options[308];
} __attribute__((packed));

namespace DHCP
{
	void Discover(NetInterface *intf);
	void HandlePacket(NetInterface *intf, IPv4_Header *ip_hdr, UDP_Packet *udp, NetPacket *pkt);
} // namespace DHCP
