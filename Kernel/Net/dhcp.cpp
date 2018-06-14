#include "dhcp.h"
#include "Lib/debug.h"

#define TYPE_DISCOVER	1
#define TYPE_OFFER		2
#define TYPE_REQUEST	3
#define TYPE_ACK		4

struct DHCP_DISCOVER
{
	uint8 op;
	uint8 htype;
	uint8 hlen;
	uint8 hops;

	uint32 xid;

	uint16 secs;
	uint16 flags;

	uint32 ciaddr;
	uint32 yiaddr;
	uint32 siaddr;
	uint32 giaddr;
	MacAddress chaddr;

	char pad[10];

	char sname[64];
	char file[128];

	uint32 magic;

	uint8 options[308];
};

void DHCP::Receive(NetInterface* intf, IPv4_Header* ip_hdr, UDP_Packet* udp, NetPacket* pkt)
{
	Debug::Print("DHCP RECEIVE\n");
}

void DHCP::DoStuff(NetInterface* intf, IPv4Address dst)
{
	DHCP_DISCOVER* d = new DHCP_DISCOVER;

	d->op = TYPE_DISCOVER;
	d->htype = 1;
	d->hlen = 6;
	d->hops = 0;
	
	d->xid = htonl(0x3903F326);

	d->secs = htons(0);
	d->flags = htons(0x8000);

	d->ciaddr = htonl(0);
	d->yiaddr = htonl(0);
	d->siaddr = htonl(0);
	d->giaddr = htonl(0);

	d->chaddr = intf->GetMac();

	d->magic = htonl(0x63825363);

	d->options[0] = 0x35;
	d->options[1] = 1;
	d->options[2] = 1;
	d->options[3] = 0xFF;

	int length = sizeof(DHCP_DISCOVER) - (308 - 4);

	Debug::Print("%i\n", length);

	NetPacket* pkt = UDP::CreatePacket(intf, dst, PORT_DHCP_DST, PORT_DHCP_SRC, (uint8*)d, length);
	intf->Send(pkt);
}
