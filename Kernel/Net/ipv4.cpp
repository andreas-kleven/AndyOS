#include "ipv4.h"
#include "net.h"
#include "arp.h"
#include "icmp.h"
#include "udp.h"
#include "tcp.h"
#include "packetmanager.h"
#include "Lib/debug.h"

namespace IPv4
{
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
		ih->flags_frag = header->flags_frag; //ntohs???
		ih->ttl = header->ttl;
		ih->protocol = header->protocol;
		ih->checksum = ntohs(header->checksum);
		ih->src = header->src;
		ih->dst = header->dst;

		pkt->end = pkt->start + ih->length;
		return 1;
	}

	NetPacket *CreatePacket(const sockaddr_in *dest_addr, uint8 protocol, uint32 size)
	{
		uint32 dst_addr = dest_addr->sin_addr.s_addr;
		NetInterface *intf = PacketManager::GetInterface(dest_addr);
		MacAddress mac;

		if (dst_addr == INADDR_BROADCAST)
		{
			mac = Net::BroadcastMAC;
		}
		else if (!memcmp(&dst_addr, &intf->gateway_addr, 3))
		{
			mac = ARP::GetMac(intf, dst_addr);
		}
		else
		{
			mac = ARP::GetMac(intf, intf->gateway_addr);
		}

		if (mac == Net::NullMAC)
			return 0;

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
		header->dst = dst_addr;

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
		//debug_print("IP PACKET\n");

		IPv4_Header header;
		if (!Decode(&header, pkt))
			return;

		uint32 intf_addr = intf->GetIP();
		if (header.dst != intf_addr && header.dst != INADDR_BROADCAST)
			return;

		ARP::AddEntry(eth->header->src, header.src);
		//Net::PrintIP("IP from: ", header.src);

		pkt->start += sizeof(IPv4_Header);

		switch (header.protocol)
		{
		case IP_PROTOCOL_ICMP:
			//ICMP::HandlePacket(intf, &header, pkt);
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
