#pragma once
#include "eth.h"

#define ARP_REQUEST		1
#define ARP_REPLY		2

#define ARP_HTYPE		1

#define ARP_CACHE_SIZE	32

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
};

struct ARP_TABLE_ENTRY
{
	MacAddress mac;
	IPv4Address ip;
};

static class ARP
{
public:
	static STATUS Init();

	static void Receive(NetInterface* intf, NetPacket* pkt);
	static bool Decode(ARP_Header* ah, NetPacket* pkt);

	static void Send(NetInterface* intf, MacAddress dst, IPv4Address tip, uint16 op);
	static void SendRequest(NetInterface* intf, IPv4Address tip);

	static void AddEntry(MacAddress mac, IPv4Address ip);
	static MacAddress ARP::LookupMac(IPv4Address ip);

private:
	static ARP_TABLE_ENTRY arp_cache[];

	static void SendReply(NetInterface* intf, ARP_Header* ah);
};