#include "kernel.h"
#include "Lib/globals.h"
#include "stdio.h"
#include "string.h"
#include "debug.h"
#include "Boot/multiboot.h"
#include "Memory/memory.h"
#include "HAL/cpu.h"
#include "Drawing/vbe.h"
#include "Drawing/drawing.h"
#include "math.h"
#include "HAL/hal.h"
#include "exceptions.h"
#include "Drivers/ata.h"
#include "PCI/pci.h"
#include "Drivers/keyboard.h"
#include "Drivers/mouse.h"
#include "Drivers/ac97.h"
#include "API/syscalls.h"

#include "Net/net.h"
//#include "i825.h"
//#include "eth.h"
#include "Drivers/ne1000.h"
//#include "udp.h"
//#include "udpsocket.h"
//#include "dhcp.h"
//#include "dns.h"
//#include "tcp.h"
//#include "tcpsocket.h"
#include "Process/scheduler.h"
#include "Kernel/os.h"

void Kernel::Setup(MULTIBOOT_INFO* bootinfo)
{
	uint32 mem_map = KERNEL_BASE_PHYS + KERNEL_SIZE;
	PMem::Init(MEMORY_SIZE, (uint32*)mem_map);
	PMem::InitRegion((uint32*)(mem_map + MEMORY_MAP_SIZE), MEMORY_SIZE - (mem_map + MEMORY_MAP_SIZE));
	VMem::Init(bootinfo);
}

void Kernel::HigherHalf(MULTIBOOT_INFO* bootinfo)
{
	CPU::Init();
	HAL::Init();
	Exceptions::Init();

	/**/VBE_MODE_INFO* vbeMode = (VBE_MODE_INFO*)bootinfo->vbe_mode_info;
	/**/VBE::Init(vbeMode);
	/**/Drawing::Init(VBE::mode.width, VBE::mode.height, VBE::mode.framebuffer);

	ATA::Init();
	
	Mouse::Init(Drawing::gc.width, Drawing::gc.height, 0.5);
	Keyboard::Init();

	Scheduler::Init();
	Syscalls::Init();

	Debug::color = 0xFF00FF00;
	PCI::Init();
	
	PCI_DEVICE* net_dev = PCI::GetDevice(2, 0, 0);

	Debug::Print("Found network card\n");

	//KNet::Init();
	//E1000* e1000 = new E1000(net_dev);
	//KNet::intf = e1000;

	//OS::Main();
	Thread* mainThread = Scheduler::CreateKernelThread(OS::Main);
	Scheduler::InsertThread(mainThread);
	Scheduler::StartThreading();

	//ARP::Init();

	/*IPv4Address g;
	//*(uint32*)&g = htonl(0xC0A80001);
	*(uint32*)&g = htonl(0xC0A83801);

	IPv4Address dns_S;
	*(uint32*)&dns_S = htonl(0x08080808);

	while (1)
	{
		Debug::Print("sending...\n");
		//DHCP::DoStuff(e1000, KNet::BroadcastIPv4);

		DNS::dns_server = dns_S;
		DNS::Query(KNet::intf, "andyhk.ga");
		while (Keyboard::GetLastKey().key == KEY_INVALID);
		Keyboard::DiscardLastKey();
	}

	while (1);*/

	char* http = "GET /test.php HTTP/1.1\r\nHost: 192.168.0.123\r\n\r\n";

	srand(PIT::ticks);

	/*IPv4Address addr = KNet::GatewayIPv4;
	addr.n[3] = 1;

	//51.175.48.225
	addr.n[0] = 0x33;
	addr.n[1] = 0xAF;
	addr.n[2] = 0x30;
	addr.n[3] = 0xE1;

	//192.168.0.123
	addr.n[0] = 0xC0;
	addr.n[1] = 0xA8;
	addr.n[2] = 0x00;
	addr.n[3] = 0x7B;

	//google.com
	//addr.n[0] = 0xD8;
	//addr.n[1] = 0x3A;
	//addr.n[2] = 0xD1;
	//addr.n[3] = 0x8E;

	//TcpSession* ses = TCP::CreateSession();
	//ses->Connect(addr, 1337);
	TcpSocket* ts = new TcpSocket;

	int port = 80;

	Debug::Print("Connecting...\n");
	bool b = ts->Connect(addr, port);

	while (1)
	{
		while (Keyboard::GetLastKey().key == KEY_INVALID || !Keyboard::GetLastKey().pressed);
		KEY_PACKET key = Keyboard::GetLastKey();
		Keyboard::DiscardLastKey();

		if (key.key == KEY_SPACE)
		{
			Debug::Print("Connecting...\n");
			bool b = ts->Connect(addr, port);
			//ses->Connect(addr, 1337);
		}

		if (key.key == KEY_S)
		{
			char buf[256];
			vprintf(buf, "OS ticks: %i", PIT::ticks);
			int length = strlen(buf);

			Debug::Print("Sending...\n");
			//ts->Send((uint8*)buf, length);
			ts->Send((uint8*)http, strlen(http));
			//ses->SendData((uint8*)buf, length);
		}

		if (key.key == KEY_ESCAPE)
		{
			bool b = ts->Close();
		}
	}

	UdpSocket socket(addr, port);
	while (1)
	{
		char* buf;
		vprintf(buf, "OS ticks: %i", PIT::ticks);
		int length = strlen(buf);

		//NetPacket* pkt = UDP::CreatePacket(e1000, tip, 1337, 1337, (uint8*)buf, length);

		while (Keyboard::GetLastKey().key == KEY_INVALID);
		Keyboard::DiscardLastKey();

		char* data = "hello ..";
		socket.Send((uint8*)data, strlen(data));

		KNet::PrintIP("Sent to ", addr);

		//if (pkt)
		//{
		//	Debug::Print("Sending...\n");
		//	e1000->Send(pkt);
		//}
		//else
		//{
		//	Debug::Print("ERROR 0\n");
		//}
	}*/

	//Debug::Print("Done\n");
	//while (1);
}