#pragma once
#include <types.h>
#include <Net/netinterface.h>
#include <Net/address.h>

uint16 htons(uint16 val);
uint16 ntohs(uint16 val);
uint32 htonl(uint32 val);
uint32 ntohl(uint32 val);

namespace Net
{
	extern MacAddress BroadcastMAC;
	extern MacAddress NullMAC;

	STATUS Init();
	uint16 Checksum(void *data, int length);
	uint16 ChecksumDouble(void *d0, int l0, void *d1, int l1);

	void PrintIP(const char *str, uint32 ip);
	void PrintMac(const char *str, MacAddress mac);
} // namespace Net
