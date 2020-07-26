#pragma once
#include <net.h>
#include <Net/packet.h>
#include <Net/netinterface.h>
#include <Net/address.h>

#define ETHERTYPE_IPv4 0x800
#define ETHERTYPE_ARP 0x806

struct EthHeader
{
	MacAddress dst;
	MacAddress src;
	uint16 type;
} __attribute__((packed));

struct EthPacket
{
	EthHeader *header;
	uint16 type;
	uint16 header_length;
} __attribute__((packed));

namespace ETH
{
	NetPacket *CreatePacket(NetInterface *intf, const MacAddress &dst, uint16 type, uint32 size);

	int Send(NetPacket *pkt);
	void HandlePacket(NetInterface *intf, NetPacket *pkt);
} // namespace ETH
