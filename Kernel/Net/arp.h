#pragma once
#include "eth.h"

#define ARP_HTYPE 1
#define ARP_CACHE_SIZE 32

struct ARP_Header
{
	uint16 htype;
	uint16 ptype;
	uint8 hlen;
	uint8 plen;
	uint16 op;
	MacAddress send_mac;
	IPv4Address send_ip;
	MacAddress recv_mac;
	IPv4Address recv_ip;
} __attribute__((packed));

namespace ARP
{
	MacAddress GetMac(NetInterface *intf, const IPv4Address &ip);
	void AddEntry(const MacAddress &mac, const IPv4Address &ip);

	void SendRequest(NetInterface *intf, const IPv4Address &ip);
	void HandlePacket(NetInterface *intf, NetPacket *pkt);

	STATUS Init();
}; // namespace ARP
