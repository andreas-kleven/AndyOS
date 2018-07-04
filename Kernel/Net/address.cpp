#include "address.h"
#include "net.h"

MacAddress::MacAddress()
{
    memset(&n, 0, 6);
}

MacAddress::MacAddress(uint8 n0, uint8 n1, uint8 n2, uint8 n3, uint8 n4, uint8 n5)
{
	n[0] = n0;
	n[1] = n1;
	n[2] = n2;
	n[3] = n3;
	n[4] = n4;
	n[5] = n5;
}

IPv4Address::IPv4Address()
{ 
    n[0] = 0;
    n[1] = 0;
    n[2] = 0;
    n[3] = 0;
}

IPv4Address::IPv4Address(uint8 n0, uint8 n1, uint8 n2, uint8 n3)
{
	n[0] = n0;
	n[1] = n1;
	n[2] = n2;
	n[3] = n3;
}

uint32 IPv4Address::ToInt()
{
    return htonl(*(uint32*)n);
}

uint32 IPv4Address::ToIntNet()
{
    return *(uint32*)n;
}