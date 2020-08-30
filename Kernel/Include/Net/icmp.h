#pragma once
#include <Net/address.h>
#include <Net/eth.h>
#include <Net/ipv4.h>
#include <Net/netinterface.h>
#include <Net/packet.h>
#include <types.h>

#define ICMP_ECHO_REPLY   0
#define ICMP_ECHO_REQUEST 8

struct ICMP_Header
{
    uint8 type;
    uint8 code;
    uint16 checksum;
    uint16 id;
    uint16 seq;
} __attribute__((packed));

struct ICMP_Packet
{
    ICMP_Header *header;
    uint8 *data;
    uint32 data_length;
} __attribute__((packed));

namespace ICMP {
void Send(ICMP_Packet *icmp, uint32 tip, uint8 type);
void SendReply(ICMP_Packet *icmp, uint32 tip);
void HandlePacket(IPv4_Header *ip_hdr, NetPacket *pkt);
} // namespace ICMP
