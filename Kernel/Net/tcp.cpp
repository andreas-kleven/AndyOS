#include "tcp.h"
#include "Lib/debug.h"
#include "HAL/hal.h"
#include "math.h"
#include "tcpsession.h"

namespace TCP
{
	TcpSession* sessions[MAX_SESSIONS];

	bool Decode(TCP_Packet* tp, NetPacket* pkt)
	{
		TCP_Header* header = (TCP_Header*)pkt->start;
		tp->header = header;

		tp->header->src_port = ntohs(header->src_port);
		tp->header->dst_port = ntohs(header->dst_port);
		tp->header->seq = ntohl(header->seq);
		tp->header->ack = ntohl(header->ack);
		tp->header->offset = header->offset;
		tp->header->flags = header->flags;
		tp->header->window_size = htons(header->window_size);
		tp->header->checksum = htons(header->checksum);
		tp->header->urg_ptr = htons(header->urg_ptr);

		tp->data = (uint8*)header + (header->offset >> 4) * 4;
		tp->data_length = pkt->end - tp->data;
		return 1;
	}

	TcpSession* CreateSession()
	{
		for (int i = 0; i < MAX_SESSIONS; i++)
		{
			if (sessions[i] == 0)
			{
				sessions[i] = new TcpSession;
				return sessions[i];
			}
		}

		return 0;
	}

	NetPacket* CreatePacket(NetInterface* intf, IPv4Address dst, uint16 src_port, uint16 dst_port, uint8 flags, uint32 seq, uint32 ack, uint8* data, uint32 data_length)
	{
		NetPacket* pkt = IPv4::CreatePacket(intf, dst, IP_PROTOCOL_TCP, sizeof(TCP_Header) + data_length);

		if (!pkt)
			return 0;

		TCP_Header* header = (TCP_Header*)pkt->end;
		header->src_port = htons(src_port);
		header->dst_port = htons(dst_port);
		header->seq = htonl(seq);
		header->ack = htonl(ack);

		header->offset = (sizeof(TCP_Header) / 4) << 4;
		header->flags = flags;
		header->window_size = ntohs(0x1000);
		header->checksum = ntohs(0);
		header->urg_ptr = ntohs(0);

		memcpy((uint8*)header + sizeof(TCP_Header), data, data_length);

		IPv4_PSEUDO_HEADER pseudo;
		pseudo.src = intf->GetIP();
		pseudo.dst = dst;
		pseudo.reserved = 0;
		pseudo.protocol = IP_PROTOCOL_TCP;
		pseudo.length = ntohs(sizeof(TCP_Header) + data_length);

		header->checksum = Net::ChecksumDouble(&pseudo, sizeof(IPv4_PSEUDO_HEADER), header, sizeof(TCP_Header) + data_length);
		pkt->end += sizeof(TCP_Header) + data_length;
		return pkt;
	}

	void Send(NetInterface* intf, NetPacket* pkt)
	{
		//checksum
		IPv4::Send(intf, pkt);
	}

	void Receive(NetInterface* intf, IPv4_Header* ip_hdr, NetPacket* pkt)
	{
		TCP_Packet tcp;
		//debug_dump(pkt->start, 127);

		if (!Decode(&tcp, pkt))
			return;

		for (int i = 0; i < MAX_SESSIONS; i++)
		{
			TcpSession* session = sessions[i];
		
			if (session)
			{
				bool valid_port = session->src_port == tcp.header->dst_port;
				bool valid_ip = session->dst_ip == ip_hdr->src || session->dst_ip == Net::BroadcastIPv4;

				if (valid_port || valid_ip)
				{
					//if(session->state)
					session->Receive(ip_hdr, &tcp);
				}
			}
		}
	}
}