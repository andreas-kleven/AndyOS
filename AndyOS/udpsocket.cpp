#include "udpsocket.h"

UdpSocket::UdpSocket(uint16 port)
{
	//intf = Net::intf;
	//
	//this->ip_address = ip_address;
	this->dst_port = port;
}

UdpSocket::~UdpSocket()
{

}

void UdpSocket::Send(IPv4Address dst, uint8* data, uint16 length)
{
	NetPacket* pkt = UDP::CreatePacket(intf, ip_address, src_port, dst_port, data, length);

	if (pkt)
		intf->Send(pkt);
}

int UdpSocket::Receive(uint8*& buffer)
{
	return 0;
}
