#include "http.h"
#include "dns.h"
#include "stdio.h"
#include "HAL/hal.h"

WebClient::WebClient()
{
}

void WebClient::Send(String addr, uint8* data, uint32 length)
{
	session = TCP::CreateSession();
	
	DNS::Query(Net::intf, addr.ToChar());
	PIT::Sleep(1000);

	IPv4Address ip = DNS::LookupAddress(addr.ToChar());

	//List<String> list;
	//addr.Split(list, '.');
	//
	//ip.n[0] = atoi(list[0].ToChar());
	//ip.n[1] = atoi(list[1].ToChar());
	//ip.n[2] = atoi(list[2].ToChar());
	//ip.n[3] = atoi(list[3].ToChar());


	Net::PrintIP("PARSED IP: ", ip);
}

int WebClient::Download(String addr, uint8*& data)
{
	return 0;
}