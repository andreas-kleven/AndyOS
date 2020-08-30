#pragma once
#include <Net/eth.h>

#define IP_PROTOCOL_ICMP 1
#define IP_PROTOCOL_TCP  6
#define IP_PROTOCOL_UDP  17

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
    uint32 src;
    uint32 dst;
} __attribute__((packed));

struct IPv4_PSEUDO_HEADER
{
    uint32 src;
    uint32 dst;
    uint8 reserved;
    uint8 protocol;
    uint16 length;
} __attribute__((packed));

namespace IPv4 {
NetPacket *CreatePacket(uint32 dst, uint8 protocol, uint32 size);
int Send(NetPacket *pkt);
void HandlePacket(NetInterface *intf, EthPacket *eth, NetPacket *pkt);
} // namespace IPv4
