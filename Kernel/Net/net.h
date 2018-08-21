#pragma once
#include "types.h"
#include "netinterface.h"
#include "netpacket.h"
#include "address.h"

uint16 htons(uint16 val);
uint16 ntohs(uint16 val);
uint32 htonl(uint32 val);
uint32 ntohl(uint32 val);

namespace Net
{
	extern MacAddress BroadcastMAC;
	extern IPv4Address BroadcastIPv4;
	extern MacAddress NullMAC;
	extern IPv4Address NullIPv4;

	STATUS Init();
	uint16 Checksum(void* data, int length);
	uint16 ChecksumDouble(void* d0, int l0, void* d1, int l1);

	void PrintIP(char* str, IPv4Address ip);
	void PrintMac(char* str, MacAddress mac);
}