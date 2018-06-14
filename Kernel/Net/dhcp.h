#pragma once
#include "udp.h"

#define PORT_DHCP_SRC	67
#define PORT_DHCP_DST	68

struct DHCP_Header
{

};

class DHCP
{
public:
	static void Receive(NetInterface* intf, IPv4_Header* ip_hdr, UDP_Packet* udp, NetPacket* pkt);
	static void DoStuff(NetInterface* intf, IPv4Address dst);
};