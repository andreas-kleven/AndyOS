#include "icmp.h"
#include "net.h"
#include "packetmanager.h"
#include "debug.h"

namespace ICMP
{
	bool Decode(ICMP_Packet *ih, NetPacket *pkt)
	{
		ICMP_Header *header = (ICMP_Header *)pkt->start;
		ih->header = header;

		ih->header->type = header->type;
		ih->header->code = header->code;
		ih->header->checksum = ntohs(header->checksum);

		if (ih->header->type == ICMP_ECHO_REQUEST)
		{
			ih->header->id = ntohs(header->id);
			ih->header->seq = ntohs(header->seq);

			ih->data = (uint8 *)header + sizeof(ICMP_Header);
			ih->data_length = pkt->end - ih->data;
			return 1;
		}
		else
		{
			return 0;
		}
	}

	void Send(ICMP_Packet *icmp, uint32 ip, uint8 type)
	{
		NetInterface *intf = PacketManager::GetInterface(ip);
		NetPacket *pkt = IPv4::CreatePacket(ip, IP_PROTOCOL_ICMP, sizeof(ICMP_Header) + icmp->data_length);

		if (!pkt)
			return;

		ICMP_Header *header = (ICMP_Header *)pkt->end;

		header->type = type;
		header->code = 0;
		header->checksum = 0;
		header->id = htons(icmp->header->id);
		header->seq = htons(icmp->header->seq);

		memcpy(header + 1, icmp->data, icmp->data_length);
		header->checksum = Net::Checksum(header, sizeof(ICMP_Header) + icmp->data_length);

		pkt->end += sizeof(ICMP_Header) + icmp->data_length;
		IPv4::Send(pkt);
	}

	void SendReply(ICMP_Packet *icmp, uint32 ip)
	{
		Send(icmp, ip, ICMP_ECHO_REPLY);
	}

	void HandlePacket(IPv4_Header *ip_hdr, NetPacket *pkt)
	{
		ICMP_Packet icmp;
		if (!Decode(&icmp, pkt))
			return;

		switch (icmp.header->type)
		{
		case ICMP_ECHO_REQUEST:
			Net::PrintIP("icmp: echo from ", ip_hdr->src);
			SendReply(&icmp, ip_hdr->src);
			break;
		}
	}
} // namespace ICMP
