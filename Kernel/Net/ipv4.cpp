#include "ipv4.h"
#include "hal.h"
#include "Kernel/timer.h"
#include "net.h"
#include "arp.h"
#include "icmp.h"
#include "udp.h"
#include "tcp.h"
#include "Lib/debug.h"

namespace IPv4
{
	struct IPv4_Packet
	{
		IPv4_Header* hdr;
		uint8* data;
		uint32 data_length;
	};

	bool Decode(IPv4_Header* ih, NetPacket* pkt)
	{
		IPv4_Header* header = (IPv4_Header*)pkt->start;
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
		return 1;
	}

	NetPacket* CreatePacket(NetInterface* intf, IPv4Address dst, uint8 protocol, uint32 size)
	{
		MacAddress mac;

		if (dst == Net::BroadcastIPv4)
		{
			mac = Net::BroadcastMAC;
		}
		else if (!memcmp(&dst, &intf->gateway_addr, 3))
		{
			mac = ARP::LookupMac(dst);

			if (mac == Net::NullMAC)
			{
				ARP::SendRequest(intf, dst);
				Timer::Sleep(100);
				//intf->Poll();
				mac = ARP::LookupMac(dst);
				
				if (mac == Net::NullMAC)
					return 0;
			}
		}

		NetPacket* pkt = ETH::CreatePacket(intf, mac, ETHERTYPE_IPv4, sizeof(IPv4_Header) + size);

		if (!pkt)
			return 0;

		IPv4_Header* header = (IPv4_Header*)pkt->end;

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

	void Send(NetInterface* intf, NetPacket* pkt)
	{
		ETH::Send(intf, pkt);
	}

	void Receive(NetInterface* intf, EthPacket* eth, NetPacket* pkt)
	{
		//debug_print("IP PACKET\n");

		IPv4_Header header;
		if (!Decode(&header, pkt))
			return;

		IPv4Address intf_addr = intf->GetIP();
		if (header.dst != intf_addr && header.dst != Net::BroadcastIPv4)
			return;

		ARP::AddEntry(eth->header->src, header.src);
		//Net::PrintIP("IP from: ", header.src);

		pkt->start += sizeof(IPv4_Header);

		switch (header.protocol)
		{
		case IP_PROTOCOL_ICMP:
			ICMP::Receive(intf, &header, pkt);
			break;
		case IP_PROTOCOL_UDP:
			UDP::Receive(intf, &header, pkt);
			break;
		case IP_PROTOCOL_TCP:
			TCP::Receive(intf, &header, pkt);
			break;
		}
	}
}