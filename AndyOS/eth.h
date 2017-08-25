#pragma once
#include "net.h"
#include "netpacket.h"
#include "netinterface.h"
#include "address.h"

#define ET_IPv4		0x800
#define ET_ARP		0x806

struct EthHeader
{
	MacAddress dst;
	MacAddress src;
	uint16 type;
};

struct EthPacket
{
	EthHeader* header;
	uint16 type;
	uint16 header_length;
};

static class Eth
{
public:
	static void Send(NetInterface* intf, NetPacket* pkt);
	static NetPacket* CreatePacket(NetInterface* intf, MacAddress dst, uint16 type, uint32 size);
	
	static void Receive(NetInterface* intf, NetPacket* pkt);
	static bool Decode(EthPacket* ep, NetPacket* pkt);
};