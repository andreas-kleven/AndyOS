#include "net.h"
#include "eth.h"
#include "arp.h"
#include "dns.h"
#include "Lib/debug.h"

uint16 htons(uint16 val)
{
	return (val >> 8) | (val << 8);

}
uint16 ntohs(uint16 val)
{
	return (val >> 8) | (val << 8);

}
uint32 htonl(uint32 val)
{
	return ((val >> 24) & 0xff) |
		((val << 8) & 0xff0000) |
		((val >> 8) & 0xff00) |
		((val << 24) & 0xff000000);
}
uint32 ntohl(uint32 val)
{
	return ((val >> 24) & 0xff) |
		((val << 8) & 0xff0000) |
		((val >> 8) & 0xff00) |
		((val << 24) & 0xff000000);
}

NetInterface* Net::intf;

MacAddress Net::GatewayMAC;
IPv4Address Net::GatewayIPv4;

MacAddress Net::BroadcastMAC;
IPv4Address Net::BroadcastIPv4;

MacAddress Net::NullMAC;
IPv4Address Net::NullIPv4;

STATUS Net::Init()
{
	ARP::Init();
	DNS::Init();
	UDP::Init();

	if (1)
	{
		//Ethernet
		uint8 mac[6] = { 0x18, 0xA6, 0xF7, 0x22, 0x56, 0x80 };
		memcpy(&GatewayMAC, mac, 6);
		*(uint32*)&GatewayIPv4 = htonl(0xC0A80001);
	}
	else
	{
		//VirtualBox
		uint8 mac[6] = { 0x0A, 0x00, 0x27, 0x00, 0x00, 0x0B };
		memcpy(&GatewayMAC, mac, 6);
		*(uint32*)&GatewayIPv4 = htonl(0xC0A83801);
	}

	memset(&BroadcastMAC, 0xFF, 6);
	memset(&BroadcastIPv4, 0xFF, 4);

	memset(&NullMAC, 0, 6);
	memset(&NullIPv4, 0, 4);

	return STATUS_SUCCESS;
}

uint16 Net::Checksum(void* vdata, int length)
{
	uint32 acc = 0xFFFF;
	uint16* data = (uint16*)vdata;

	while (length > 0)
	{
		acc += htons(*data++);

		if (acc > 0xFFFF)
			acc -= 0xFFFF;

		length -= 2;
	}

	if (length) 
	{
		acc += htons(*data & 0xFF);
		if (acc > 0xFFFF)
			acc -= 0xFFFF;
	}

	return htons(~acc);
}

uint16 Net::ChecksumDouble(void* d0, int l0, void* d1, int l1)
{
	uint32 acc = 0xFFFF;
	uint16* data0 = (uint16*)d0;
	uint16* data1 = (uint16*)d1;

	while (l0 > 0)
	{
		acc += htons(*data0++);

		if (acc > 0xFFFF)
			acc -= 0xFFFF;

		l0 -= 2;
	}

	if (l0)
	{
		acc += (*data0 << 4) + (*data1++ & 0xFF);
		l1++;
	}

	while (l1 > 0)
	{
		acc += htons(*data1++);

		if (acc > 0xFFFF)
			acc -= 0xFFFF;

		l1 -= 2;
	}

	if ((l0 + l1) & 1)
	{
		acc += htons(*data1 & 0xFF);
		if (acc > 0xFFFF)
			acc -= 0xFFFF;
	}

	return htons(~acc);
}

void Net::PrintIP(char* str, IPv4Address ip)
{
	uint8* addr = (uint8*)&ip;
	Debug::Print("%s%i.%i.%i.%i\n", str, addr[0], addr[1], addr[2], addr[3]);
}

void Net::PrintMac(char* str, MacAddress mac)
{
	uint8* addr = (uint8*)&mac;
	Debug::Print("%s%x:%x:%x:%x:%x:%x\n", str, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}
