#include "udpsocket.h"
#include "math.h"

UdpSocket::UdpSocket(uint16 src_port, uint16 dst_port)
{
	intf = Net::intf;
	
	this->src_port = src_port;
	this->dst_port = dst_port;

	pkt_buffer = new uint8[UDP_MAX_PACKET_SIZE];
}

UdpSocket::~UdpSocket()
{

}

void UdpSocket::Send(IPv4Address dst, uint8* data, uint16 length)
{
	NetPacket* pkt = UDP::CreatePacket(intf, dst, src_port, dst_port, data, length);

	if (pkt)
		UDP::Send(intf, pkt);
}

int UdpSocket::Receive(uint8*& buffer, IPv4Address& addr)
{
	pkt_length = 0;

	while (pkt_length == 0)
	{
		_asm pause;
	}

	buffer = new uint8[UDP_MAX_PACKET_SIZE];
	memcpy(buffer, pkt_buffer, pkt_length);
	return pkt_length;
}

void UdpSocket::SetReceivedData(IPv4Address addr, uint8* buffer, uint16 length)
{
	memcpy(pkt_buffer, buffer, length);
	pkt_length = length;
}
