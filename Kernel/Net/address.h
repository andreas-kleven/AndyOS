#pragma once
#include "definitions.h"
#include "string.h"

struct MacAddress
{
	uint8 n[6];

	MacAddress()
	{ }

	MacAddress(uint8 n0, uint8 n1, uint8 n2, uint8 n3, uint8 n4, uint8 n5)
	{
		n[0] = n0;
		n[1] = n1;
		n[2] = n2;
		n[3] = n3;
		n[4] = n4;
		n[5] = n5;
	}

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

	IPv4Address() 
	{ }

	IPv4Address(uint8 n0, uint8 n1, uint8 n2, uint8 n3)
	{
		n[0] = n0;
		n[1] = n1;
		n[2] = n2;
		n[3] = n3;
	}

	bool operator==(IPv4Address& a)
	{
		return *(uint32*)&n == *(uint32*)&a.n;
	}

	bool operator!=(IPv4Address& a)
	{
		return *(uint32*)&n != *(uint32*)&a.n;
	}
};