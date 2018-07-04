#include "net.h"
#include "udp.h"
#include "dhcp.h"
#include "dns.h"
#include "udpsocket.h"
#include "Lib/debug.h"

namespace UDP
{
	UdpSocket** sockets;

	bool Decode(UDP_Packet* up, NetPacket* pkt)
	{
		UDP_Header* header = (UDP_Header*)pkt->start;
		up->header = header;

		header->src_port = ntohs(header->src_port);
		header->dst_port = ntohs(header->dst_port);
		header->length = ntohs(header->length);
		header->checksum = ntohs(header->checksum);

		up->data = (uint8*)header + sizeof(UDP_Header);
		up->data_length = up->header->length - sizeof(UDP_Header);
		return 1;
	}

	UdpSocket* CreateSocket(int port)
	{
		int src_port = 123;
		UdpSocket* socket = new UdpSocket(src_port, port);

		if (sockets[port])
			return 0;

		sockets[port] = socket;
		return socket;
	}

	NetPacket* CreatePacket(NetInterface* intf, IPv4Address dst, uint16 src_port, uint16 dst_port, uint8* data, uint32 data_length)
	{
		NetPacket* pkt = IPv4::CreatePacket(intf, dst, IP_PROTOCOL_UDP, sizeof(UDP_Header) + data_length);

		if (!pkt)
			return 0;

		UDP_Header* header = (UDP_Header*)pkt->end;
		header->src_port = htons(src_port);
		header->dst_port = htons(dst_port);
		header->length = htons(sizeof(UDP_Header) + data_length);
		header->checksum = 0;

		memcpy(header + 1, data, data_length);

		IPv4_PSEUDO_HEADER pseudo;
		pseudo.src = intf->GetIP();
		pseudo.dst = dst;
		pseudo.reserved = 0;
		pseudo.protocol = IP_PROTOCOL_UDP;
		pseudo.length = ntohs(sizeof(UDP_Header) + data_length);

		header->checksum = Net::ChecksumDouble(&pseudo, sizeof(IPv4_PSEUDO_HEADER), header, sizeof(UDP_Header) + data_length);
		pkt->end += sizeof(UDP_Header) + data_length;
		return pkt;
	}

	void Send(NetInterface* intf, NetPacket* pkt)
	{
		//checksum
		IPv4::Send(intf, pkt);
	}

	void Receive(NetInterface* intf, IPv4_Header* ip_hdr, NetPacket* pkt)
	{
		//Net::PrintIP("UDP from: ", ip_hdr->src);

		UDP_Packet udp;
		if (!Decode(&udp, pkt))
			return;

		//debug_dump(udp.data, udp.data_length, 1);
		pkt->start += udp.header->length;

		uint16 src_port = udp.header->src_port;
		uint16 dst_port = udp.header->dst_port;

		switch (udp.header->src_port)
		{
		case PORT_DNS:
			DNS::Receive(intf, ip_hdr, &udp, pkt);
			return;

		case PORT_DHCP_SRC:
			DHCP::Receive(intf, ip_hdr, &udp, pkt);
			return;

		default:
			break;
		}

		switch (dst_port)
		{
		default:
			UdpSocket* socket = sockets[dst_port];
			if (socket)
			{
				socket->SetReceivedData(ip_hdr->src, udp.data, udp.data_length);
			}
			break;
		}
	}

	STATUS Init()
	{
		sockets = new UdpSocket*[UDP_MAX_PORTS];
		memset(sockets, 0, sizeof(UdpSocket*) * UDP_MAX_PORTS);
		return STATUS_SUCCESS;
	}
}