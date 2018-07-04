#pragma once
#include "net.h"
#include "netpacket.h"
#include "netinterface.h"
#include "address.h"

#define ETHERTYPE_IPv4		0x800
#define ETHERTYPE_ARP		0x806

struct EthHeader
{
	MacAddress dst;
	MacAddress src;
	uint16 type;
} __attribute__((packed));

struct EthPacket
{
	EthHeader* header;
	uint16 type;
	uint16 header_length;
} __attribute__((packed));

namespace ETH
{
	NetPacket* CreatePacket(NetInterface* intf, MacAddress dst, uint16 type, uint32 size);
	
	void Send(NetInterface* intf, NetPacket* pkt);
	void Receive(NetInterface* intf, NetPacket* pkt);
}