#pragma once
#include "udp.h"

#define PORT_DNS		53
#define DNS_CACHE_SIZE	64

struct DNS_Answer
{
	uint16 name;
	uint16 type;
	uint16 clas;
	uint32 ttl;
	uint16 data_length;
	IPv4Address addr;
} __attribute__((packed));

struct DNS_Header
{
	uint16 id;
	uint16 flags;
	uint16 quest_count;
	uint16 ans_count;
	uint16 auth_count;
	uint16 add_count;
} __attribute__((packed));

struct DNS_Packet
{
	DNS_Header* hdr;
	uint8* data;
	uint32 data_length;
} __attribute__((packed));

struct DNS_TABLE_ENTRY
{
	char* name;
	IPv4Address addr;
};

class DNS
{
public:
	static STATUS Init();

	static IPv4Address dns_server;

	static void Receive(NetInterface* intf, IPv4_Header* ip_hdr, UDP_Packet* udp, NetPacket* pkt);
	static bool Decode(DNS_Packet* dns, UDP_Packet* pkt);
	static void Query(NetInterface* intf, char* name);

	static void AddEntry(char* name, IPv4Address addr);
	static IPv4Address LookupAddress(char* name);
	
private:
	static DNS_TABLE_ENTRY dns_cache[];

	static uint8* AppendDomain(uint8* ptr, char* name);
	static uint8* ParseDomain(char* buf, uint8* ptr);
	static uint8* ParseAnswer(DNS_Answer* ans, uint8* ptr);
};
