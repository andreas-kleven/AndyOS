#include "net.h"
#include "eth.h"
#include "arp.h"
#include "udp.h"
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

namespace Net
{
	MacAddress BroadcastMAC;
	MacAddress NullMAC;

	STATUS Init()
	{
		ARP::Init();
		UDP::Init();

		memset(&BroadcastMAC, 0xFF, 6);
		memset(&NullMAC, 0, 6);

		return STATUS_SUCCESS;
	}

	uint16 Checksum(void* vdata, int length)
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

	uint16 ChecksumDouble(void* d0, int l0, void* d1, int l1)
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

	void PrintIP(const char* str, uint32 ip)
	{
		uint8* addr = (uint8*)&ip;
		debug_print("%s%i.%i.%i.%i\n", str, addr[0], addr[1], addr[2], addr[3]);
	}

	void PrintMac(const char* str, MacAddress mac)
	{
		uint8* addr = (uint8*)&mac;
		debug_print("%s%x:%x:%x:%x:%x:%x\n", str, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	}
}