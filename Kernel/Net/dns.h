#pragma once
#include "udp.h"

#define PORT_DNS 53
#define DNS_CACHE_SIZE 64

struct DNS_Answer
{
	uint16 name;
	uint16 type;
	uint16 clas;
	uint32 ttl;
	uint16 data_length;
	IPv4Address addr;
} __attribute__((packed));

struct DNS_Header
{
	uint16 id;
	uint16 flags;
	uint16 quest_count;
	uint16 ans_count;
	uint16 auth_count;
	uint16 add_count;
} __attribute__((packed));

namespace DNS
{
	IPv4Address LookupAddress(char *name);
	void AddEntry(char *name, IPv4Address addr);

	void Query(NetInterface *intf, char *name);
	void HandlePacket(NetInterface *intf, IPv4_Header *ip_hdr, UDP_Packet *udp, NetPacket *pkt);

	STATUS Init();
}; // namespace DNS
