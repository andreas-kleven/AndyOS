#pragma once
#include "definitions.h"
#include "netinterface.h"
#include "netpacket.h"
#include "address.h"

uint16 htons(uint16 val);
uint16 ntohs(uint16 val);
uint32 htonl(uint32 val);
uint32 ntohl(uint32 val);

static class Net
{
public:
	static NetInterface* intf;

	static IPv4Address GatewayIPv4;
	static MacAddress GatewayMAC;

	static MacAddress BroadcastMAC;
	static IPv4Address BroadcastIPv4;

	static MacAddress NullMAC;
	static IPv4Address NullIPv4;

	static STATUS Init();
	static uint16 Checksum(void* data, int length);
	static uint16 ChecksumDouble(void* d0, int l0, void* d1, int l1);

	static void PrintIP(char* str, IPv4Address ip);
	static void PrintMac(char* str, MacAddress mac);
};