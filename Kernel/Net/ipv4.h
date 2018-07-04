#pragma once
#include "eth.h"

#define IP_PROTOCOL_ICMP	1
#define IP_PROTOCOL_TCP		6
#define IP_PROTOCOL_UDP		17

struct IPv4_Header
{
	uint8 ver_ihl;
	uint8 dscp_ecn;
	uint16 length;
	uint16 id;
	uint16 flags_frag;
	uint8 ttl;
	uint8 protocol;
	uint16 checksum;
	IPv4Address src;
	IPv4Address dst;
} __attribute__((packed));

struct IPv4_PSEUDO_HEADER
{
	IPv4Address src;
	IPv4Address dst;
	uint8 reserved;
	uint8 protocol;
	uint16 length;
} __attribute__((packed));

namespace IPv4
{
	NetPacket* CreatePacket(NetInterface* intf, IPv4Address dst, uint8 protocol, uint32 size);
	void Send(NetInterface* intf, NetPacket* pkt);
	void Receive(NetInterface* intf, EthPacket* eth, NetPacket* pkt);
}