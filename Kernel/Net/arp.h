#pragma once
#include "eth.h"

#define ARP_HTYPE 1
#define ARP_CACHE_SIZE 32
#define ARP_TIMEOUT 3000

struct ARP_Header
{
	uint16 htype;
	uint16 ptype;
	uint8 hlen;
	uint8 plen;
	uint16 op;
	MacAddress send_mac;
	uint32 send_ip;
	MacAddress recv_mac;
	uint32 recv_ip;
} __attribute__((packed));

namespace ARP
{
	MacAddress GetMac(NetInterface *intf, uint32 ip);
	void AddEntry(const MacAddress &mac, uint32 ip);

	void SendRequest(NetInterface *intf, uint32 ip);
	void HandlePacket(NetInterface *intf, NetPacket *pkt);

	STATUS Init();
}; // namespace ARP
