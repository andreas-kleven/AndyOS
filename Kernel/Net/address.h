#pragma once
#include "definitions.h"
#include "string.h"

struct MacAddress
{
	uint8 n[6];

	bool operator==(const MacAddress& a)
	{
		return !memcmp(n, a.n, 6);
	}

	bool operator!=(const MacAddress& a)
	{
		return  memcmp(n, a.n, 6);
	}
};

struct IPv4Address
{
	uint8 n[4];
	//uint32 bits;

	bool operator==(IPv4Address& a)
	{
		return *(uint32*)&n == *(uint32*)&a.n;
	}

	bool operator!=(IPv4Address& a)
	{
		return *(uint32*)&n != *(uint32*)&a.n;
	}
};