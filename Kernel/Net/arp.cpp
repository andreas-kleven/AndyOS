#include <Kernel/timer.h>
#include <Net/arp.h>
#include <Process/scheduler.h>
#include <debug.h>
#include <net.h>
#include <sync.h>

#define ARP_REQUEST 1
#define ARP_REPLY   2

namespace ARP {
Mutex table_mutex;
Event lookup_event = Event(false, true);

struct ARP_TABLE_ENTRY
{
    MacAddress mac;
    uint32 ip;
};

ARP_TABLE_ENTRY arp_cache[ARP_CACHE_SIZE];

void Send(NetInterface *intf, MacAddress dst, uint32 ip, uint16 op)
{
    NetPacket *pkt = ETH::CreatePacket(intf, dst, ETHERTYPE_ARP, sizeof(ARP_Header));

    if (!pkt)
        return;

    ARP_Header *header = (ARP_Header *)pkt->end;

    header->htype = htons(ARP_HTYPE);
    header->ptype = htons(ETHERTYPE_IPv4);
    header->hlen = 6;
    header->plen = 4;
    header->op = htons(op);
    header->send_mac = intf->GetMac();
    header->send_ip = intf->GetIP();
    header->recv_mac = dst;
    header->recv_ip = ip;

    pkt->end += sizeof(ARP_Header);
    intf->Send(pkt);
}

void SendReply(NetInterface *intf, ARP_Header *ah)
{
    Net::PrintIP("arp: sending reply to ", ah->send_ip);
    Send(intf, ah->send_mac, ah->send_ip, ARP_REPLY);
}

bool Decode(ARP_Header *ah, NetPacket *pkt)
{
    ARP_Header *hdr = (ARP_Header *)pkt->start;

    ah->htype = ntohs(hdr->htype);
    ah->ptype = ntohs(hdr->ptype);
    ah->hlen = 6;
    ah->plen = 4;
    ah->op = ntohs(hdr->op);
    ah->send_mac = hdr->send_mac;
    ah->send_ip = hdr->send_ip;
    ah->recv_mac = hdr->recv_mac;
    ah->recv_ip = hdr->recv_ip;

    return 1;
}

MacAddress LookupMac(uint32 ip)
{
    if (ip == INADDR_BROADCAST)
        return Net::BroadcastMAC;

    table_mutex.Aquire();

    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].ip == ip) {
            table_mutex.Release();
            return arp_cache[i].mac;
        }
    }

    table_mutex.Release();

    return Net::NullMAC;
}

MacAddress GetMac(NetInterface *intf, uint32 ip)
{
    MacAddress mac = ARP::LookupMac(ip);

    if (mac != Net::NullMAC)
        return mac;

    ARP::SendRequest(intf, ip);

    while (true) {
        if (lookup_event.Wait(ARP_TIMEOUT)) {
            MacAddress mac = ARP::LookupMac(ip);

            if (mac != Net::NullMAC)
                return mac;
        } else {
            kprintf("ARP timeout\n");
            return Net::NullMAC;
        }
    }
}

void AddEntry(const MacAddress &mac, uint32 ip)
{
    bool add = LookupMac(ip) == Net::NullMAC;
    bool replace = !add;

    table_mutex.Aquire();

    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if ((add && arp_cache[i].mac == Net::NullMAC) || (replace && arp_cache[i].ip == ip)) {
            if (arp_cache[i].mac == Net::NullMAC && ip != htonl(LOOPBACK_ADDRESS)) {
                Net::PrintIP("ARP added entry ", ip);
                // Net::PrintMac("MAC: ", mac);
            }

            arp_cache[i].mac = mac;
            arp_cache[i].ip = ip;

            lookup_event.Set();
            table_mutex.Release();
            return;
        }
    }

    table_mutex.Release();
    kprintf("ARP add entry failed\n");
}

void HandlePacket(NetPacket *pkt)
{
    ARP_Header header;
    if (!Decode(&header, pkt))
        return;

    if (header.recv_ip != pkt->interface->GetIP())
        return;

    switch (header.op) {
    case ARP_REQUEST:
        SendReply(pkt->interface, &header);
        break;
    case ARP_REPLY:
        AddEntry(header.send_mac, header.send_ip);
        break;
    }
}

void SendRequest(NetInterface *intf, uint32 ip)
{
    Net::PrintIP("arp: sending request to ", ip);
    Send(intf, Net::BroadcastMAC, ip, ARP_REQUEST);
}

STATUS Init()
{
    memset(arp_cache, 0, sizeof(arp_cache));
    return STATUS_SUCCESS;
}
} // namespace ARP
