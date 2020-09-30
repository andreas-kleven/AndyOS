#include <Net/arp.h>
#include <Net/icmp.h>
#include <Net/ipv4.h>
#include <Net/packetmanager.h>
#include <Net/tcp.h>
#include <Net/udp.h>
#include <debug.h>
#include <net.h>

namespace IPv4 {
struct IPv4_Packet
{
    IPv4_Header *hdr;
    uint8 *data;
    uint32 data_length;
};

bool Decode(IPv4_Header *ih, NetPacket *pkt)
{
    IPv4_Header *header = (IPv4_Header *)pkt->start;
    ih->ver_ihl = header->ver_ihl;
    ih->dscp_ecn = header->dscp_ecn;
    ih->length = ntohs(header->length);
    ih->id = ntohs(header->id);
    ih->flags_frag = header->flags_frag; // ntohs???
    ih->ttl = header->ttl;
    ih->protocol = header->protocol;
    ih->checksum = ntohs(header->checksum);
    ih->src = header->src;
    ih->dst = header->dst;

    pkt->end = pkt->start + ih->length;
    return 1;
}

NetPacket *CreatePacket(uint32 dst, uint8 protocol, uint32 size)
{
    NetInterface *intf = PacketManager::GetInterface(dst);
    uint32 mask = intf->GetMask();
    MacAddress mac;

    if (dst == INADDR_BROADCAST) {
        mac = Net::BroadcastMAC;
    } else if (intf->GetMac() == Net::NullMAC) {
        mac = Net::NullMAC;

        if ((dst & mask) != (intf->GetIP() & mask)) {
            dst = intf->GetIP(); // loopback
        }
    } else {
        if ((dst & mask) == (intf->GetIP() & mask)) {
            mac = ARP::GetMac(intf, dst);
        } else {
            if (!intf->gateway_addr) {
                debug_print("No gateway\n", dst);
                return 0;
            }

            mac = ARP::GetMac(intf, intf->gateway_addr);
        }

        if (mac == Net::NullMAC)
            return 0;
    }

    NetPacket *pkt = ETH::CreatePacket(intf, mac, ETHERTYPE_IPv4, sizeof(IPv4_Header) + size);

    if (!pkt)
        return 0;

    IPv4_Header *header = (IPv4_Header *)pkt->end;

    header->ver_ihl = 0x45;
    header->dscp_ecn = 0;
    header->length = htons(sizeof(IPv4_Header) + size);
    header->id = htons(42);
    header->flags_frag = htons(0);
    header->ttl = 0x80;
    header->protocol = protocol;
    header->checksum = 0;
    header->src = intf->GetIP();
    header->dst = dst;

    header->checksum = Net::Checksum(header, sizeof(IPv4_Header));
    pkt->end += sizeof(IPv4_Header);

    return pkt;
}

int Send(NetPacket *pkt)
{
    return ETH::Send(pkt);
}

void HandlePacket(NetInterface *intf, EthPacket *eth, NetPacket *pkt)
{
    // debug_print("IP PACKET\n");

    IPv4_Header header;
    if (!Decode(&header, pkt))
        return;

    uint32 intf_addr = intf->GetIP();
    if (header.dst != intf_addr && header.dst != INADDR_BROADCAST)
        return;

    ARP::AddEntry(eth->header->src, header.src);
    // Net::PrintIP("IP from: ", header.src);

    pkt->start += sizeof(IPv4_Header);

    switch (header.protocol) {
    case IP_PROTOCOL_ICMP:
        ICMP::HandlePacket(&header, pkt);
        break;
    case IP_PROTOCOL_UDP:
        UDP::HandlePacket(&header, pkt);
        break;
    case IP_PROTOCOL_TCP:
        TCP::HandlePacket(&header, pkt);
        break;
    }
}
} // namespace IPv4
