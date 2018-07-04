#pragma once
#include "definitions.h"
#include "string.h"

struct MacAddress
{
	uint8 n[6];

	MacAddress();
	MacAddress(uint8 n0, uint8 n1, uint8 n2, uint8 n3, uint8 n4, uint8 n5);

	bool operator==(const MacAddress& a)
	{
		return !memcmp(n, a.n, 6);
	}

	bool operator!=(const MacAddress& a)
	{
		return  memcmp(n, a.n, 6);
	}
} __attribute__((packed));

struct IPv4Address
{
	uint8 n[4];

	IPv4Address();
	IPv4Address(uint8 n0, uint8 n1, uint8 n2, uint8 n3);

	uint32 ToInt();
	uint32 ToIntNet();

	bool operator==(IPv4Address& a)
	{
		return *(uint32*)&n == *(uint32*)&a.n;
	}

	bool operator!=(IPv4Address& a)
	{
		return *(uint32*)&n != *(uint32*)&a.n;
	}
} __attribute__((packed));