#pragma once
#include <Net/ipv4.h>

class UdpSocket;

#define UDP_MAX_PORTS       65536
#define UDP_MAX_PACKET_SIZE 65536

struct UDP_Header
{
    uint16 src_port;
    uint16 dst_port;
    uint16 length;
    uint16 checksum;
} __attribute__((packed));

struct UDP_Packet
{
    UDP_Header *header;
    uint16 data_length;
    uint8 *data;
} __attribute__((packed));

namespace UDP {
NetPacket *CreatePacket(NetInterface *intf, const sockaddr_in *dest_addr, uint16 src_port,
                        const void *data, size_t len);

int Send(NetPacket *pkt);
void HandlePacket(IPv4_Header *ip_hdr, NetPacket *pkt);

STATUS Init();
} // namespace UDP
