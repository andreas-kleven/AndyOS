#include <Net/eth.h>
#include <Net/arp.h>
#include <Net/ipv4.h>
#include <Net/packetmanager.h>
#include <string.h>
#include <debug.h>

namespace ETH
{
	bool Decode(EthPacket *ep, NetPacket *pkt)
	{
		uint8 *data = pkt->start;
		EthHeader *hdr = (EthHeader *)data;
		ep->header = hdr;

		// Determine which frame type is being used.
		uint16 n = ntohs(hdr->type);
		if (n <= 1500 && pkt->start + 22 <= pkt->end)
		{
			// 802.2/802.3 encapsulation (RFC 1042)
			uint8 dsap = data[14];
			uint8 ssap = data[15];

			// Validate service access point
			if (dsap != 0xaa || ssap != 0xaa)
			{
				return 0;
			}

			ep->type = (data[20] << 8) | data[21];
			ep->header_length = 22;
		}
		else
		{
			// Ethernet encapsulation (RFC 894)
			ep->type = n;
			ep->header_length = sizeof(EthHeader);
		}

		return 1;
	}

	NetPacket *CreatePacket(NetInterface *intf, const MacAddress &dst, uint16 type, uint32 size)
	{
		NetPacket *pkt = new NetPacket();
		pkt->start = new uint8[sizeof(EthHeader) + size];
		pkt->end = pkt->start + sizeof(EthHeader);
		pkt->length = sizeof(EthHeader) + size;

		EthHeader *header = (EthHeader *)pkt->start;
		header->dst = dst;
		header->src = intf->GetMac();
		header->type = htons(type);
		return pkt;
	}

	int Send(NetPacket *pkt)
	{
		return PacketManager::Send(pkt);
	}

	void HandlePacket(NetInterface *intf, NetPacket *pkt)
	{
		EthPacket ep;
		if (!Decode(&ep, pkt))
			return;

		pkt->start += ep.header_length;

		switch (ep.type)
		{
		case ETHERTYPE_IPv4:
			IPv4::HandlePacket(intf, &ep, pkt);
			break;

		case ETHERTYPE_ARP:
			ARP::HandlePacket(intf, pkt);
			break;
		}
	}
} // namespace ETH
