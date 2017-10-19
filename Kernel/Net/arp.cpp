#include "arp.h"
#include "net.h"
#include "debug.h"

ARP_TABLE_ENTRY ARP::arp_cache[ARP_CACHE_SIZE];

STATUS ARP::Init()
{
	ARP_TABLE_ENTRY entry;
	entry.ip = Net::NullIPv4;
	entry.mac = Net::NullMAC;

	for (int i = 0; i < ARP_CACHE_SIZE; i++)
		arp_cache[i] = entry;

	return STATUS_SUCCESS;
}

void ARP::Receive(NetInterface* intf, NetPacket* pkt)
{
	ARP_Header header;
	if (!Decode(&header, pkt))
		return;

	if (header.recv_ip != intf->GetIP())
		return;

	switch (header.op)
	{
	case ARP_REQUEST:
		SendReply(intf, &header);
		break;
	case ARP_REPLY:
		AddEntry(header.send_mac, header.send_ip);
		break;
	}
}

bool ARP::Decode(ARP_Header* ah, NetPacket* pkt)
{
	ARP_Header* hdr = (ARP_Header*)pkt->start;

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

void ARP::Send(NetInterface* intf, MacAddress dst, IPv4Address tip, uint16 op)
{
	NetPacket* pkt = Eth::CreatePacket(intf, dst, ET_ARP, sizeof(ARP_Header));

	if (!pkt)
		return;

	ARP_Header* header = (ARP_Header*)pkt->end;

	header->htype = htons(ARP_HTYPE);
	header->ptype = htons(ET_IPv4);
	header->hlen = 6;
	header->plen = 4;
	header->op = htons(op);
	header->send_mac = intf->GetMac();
	header->send_ip = intf->GetIP();
	header->recv_mac = dst;
	header->recv_ip = tip;

	pkt->end += sizeof(ARP_Header);
	intf->Send(pkt);
}

void ARP::SendRequest(NetInterface* intf, IPv4Address tip)
{
	Net::PrintIP("arp: sending request to ", tip);
	Send(intf, Net::BroadcastMAC, tip, ARP_REQUEST);
}

void ARP::SendReply(NetInterface* intf, ARP_Header* ah)
{
	Net::PrintIP("arp: sending reply to ", ah->send_ip);
	Send(intf, ah->send_mac, ah->send_ip, ARP_REPLY);
}

void ARP::AddEntry(MacAddress mac, IPv4Address ip)
{
	for (int i = 0; i < ARP_CACHE_SIZE; i++)
	{
		if (arp_cache[i].mac == Net::NullMAC || arp_cache[i].mac == mac)
		{
			if (arp_cache[i].mac == Net::NullMAC)
				Net::PrintIP("ARP added entry ", ip);

			arp_cache[i].mac = mac;
			arp_cache[i].ip = ip;
			break;
		}
	}
}

MacAddress ARP::LookupMac(IPv4Address ip)
{
	if (ip == Net::BroadcastIPv4)
		return Net::BroadcastMAC;

	for (int i = 0; i < ARP_CACHE_SIZE; i++)
	{
		if (arp_cache[i].ip == ip)
		{
			return arp_cache[i].mac;
		}
	}

	return Net::NullMAC;
}