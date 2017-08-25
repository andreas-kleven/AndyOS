#include "net.h"
#include "eth.h"
#include "arp.h"
#include "dns.h"

STATUS Net::Init()
{
	ARP::Init();
	DNS::Init();
	return STATUS_SUCCESS;
}

void NetEthReceive(NetInterface* intf, NetPacket* pkt)
{
	Eth::Receive(intf, pkt);
}
