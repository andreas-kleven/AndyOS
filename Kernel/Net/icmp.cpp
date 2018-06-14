#include "icmp.h"
#include "net.h"
#include "Lib/debug.h"

void ICMP::Receive(NetInterface* intf, IPv4_Header* ip_hdr, NetPacket* pkt)
{
	ICMP_Packet icmp;
	if (!Decode(&icmp, pkt))
		return;

	switch (icmp.header->type)
	{
	case ICMP_ECHO_REQUEST:
		Net::PrintIP("icmp: echo from ", ip_hdr->src);
		SendReply(intf, &icmp, ip_hdr->src);
		break;
	}
}

bool ICMP::Decode(ICMP_Packet* ih, NetPacket* pkt)
{
	ICMP_Header* header = (ICMP_Header*)pkt->start;
	ih->header = header;

	ih->header->type = header->type;
	ih->header->code = header->code;
	ih->header->checksum = ntohs(header->checksum);

	if (ih->header->type == ICMP_ECHO_REQUEST)
	{
		ih->header->id = ntohs(header->id);
		ih->header->seq = ntohs(header->seq);

		ih->data = (uint8*)header + sizeof(ICMP_Header);
		ih->data_length = pkt->end - ih->data;
	}
	else
	{
		return 0;
	}
}

void ICMP::Send(NetInterface* intf, ICMP_Packet* icmp, IPv4Address tip, uint8 type)
{
	NetPacket* pkt = IPv4::CreatePacket(intf, tip, IP_PROTOCOL_ICMP, sizeof(ICMP_Header) + icmp->data_length);

	if (!pkt)
		return;

	ICMP_Header* header = (ICMP_Header*)pkt->end;

	header->type = type;
	header->code = 0;
	header->checksum = 0;
	header->id = htons(icmp->header->id);
	header->seq = htons(icmp->header->seq);

	memcpy(header + 1, icmp->data, icmp->data_length);
	header->checksum = Net::Checksum(header, sizeof(ICMP_Header) + icmp->data_length);

	pkt->end += sizeof(ICMP_Header) + icmp->data_length;
	intf->Send(pkt);
}

void ICMP::SendReply(NetInterface* intf, ICMP_Packet* icmp, IPv4Address tip)
{
	Send(intf, icmp, tip, ICMP_ECHO_REPLY);
}
