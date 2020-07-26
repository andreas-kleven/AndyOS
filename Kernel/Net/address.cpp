#include <net.h>
#include <Net/address.h>

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
