#include "OS.h"
#include "../3DGame/3DGame.h"
#include "panic.h"
#include "stdio.h"
#include "net.h"
#include "tcpsocket.h"
#include "udpsocket.h"
#include "dns.h"
#include "dhcp.h"
#include "http.h"
#include "task.h"
#include "GUI.h"

#include "Mandelbrot.h"
#include "TextEdit.h"

void OS::Main()
{
	ISO_FS::Init();
	//Net::Init();

	//Mandelbrot::Create(100, 100);
	
	gui::WindowManager::Start();
	apps::RunTextEdit();

	//Task::InsertThread(Task::CreateThread(apps::RunTextEdit));

	///
	while (1)
	{
		MyGame* game = new MyGame();
		GEngine* engine = new GEngine();
		engine->StartGame(game);
	}
	return;
	///

	PIT::Sleep(2000);
	IPv4Address addr = Net::GatewayIPv4;
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

	//Debug::Print("Mem: %ui\n", Memory::num_free);
	//char* c = new char[123];
	//Debug::Print("Mem: %ui\n", Memory::num_free);
	//c = new char[123];
	//Debug::Print("Mem: %ui\n", Memory::num_free);
	//c = new char[0xFFF];
	//Debug::Print("Mem: %ui\n", Memory::num_free);
	//while (1);
	//google.com
	//addr.n[0] = 0xD8;
	//addr.n[1] = 0x3A;
	//addr.n[2] = 0xD1;
	//addr.n[3] = 0x8E;

	//UdpSocket us(addr, 1337);
	//
	//while (1)
	//{
	//	while (Keyboard::GetLastKey().key == KEY_INVALID || !Keyboard::GetLastKey().pressed);
	//	KEY_PACKET key = Keyboard::GetLastKey();
	//	Keyboard::DiscardLastKey();
	//
	//	char* data = "hello ..";
	//	us.Send((uint8*)data, strlen(data));
	//
	//	Debug::Print("Sent\n");
	//}


	int port = 80;
	char* http = "GET /test.php HTTP/1.1\r\nHost: 192.168.0.123\r\n\r\n";

	while (1)
	{
		//DNS::Query(Net::intf, "andyhk.ga");
		//Debug::Print("Sent DNS\n");

		//DHCP::DoStuff(Net::intf, Net::BroadcastIPv4);
		WebClient client;
		client.Send("andyhk.ga", (uint8*)http, strlen(http));

		while (Keyboard::GetLastKey().key == KEY_INVALID || !Keyboard::GetLastKey().pressed);
		KEY_PACKET key = Keyboard::GetLastKey();
		Keyboard::DiscardLastKey();

		//Debug::Print("Mem: %ui\n", Memory::num_free);
		//while (1);
	}

	TcpSocket* ts = new TcpSocket;

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
		}

		if (key.key == KEY_S)
		{
			char buf[256];
			vprintf(buf, "OS ticks: %i", PIT::ticks);
			int length = strlen(buf);

			Debug::Print("Sending...\n");
			ts->Send((uint8*)http, strlen(http));
		}

		if (key.key == KEY_ESCAPE)
		{
			bool b = ts->Close();
		}
	}

	/*char* buffer;
	ISO_DIRECTORY* file = ISO_FS::FindFile("fox.bmp");
	//ISO_FS::ReadFile("mountain.bmp", buffer);
	//ATA::Read(ATA_BUS_PRIMARY, ATA_DRIVE_MASTER, 0xAC0, buffer, 1);

	ATA::Read(ATA_BUS_PRIMARY, ATA_DRIVE_MASTER, file->locationLBA_LSB, buffer, file->filesize_LSB);
	//Debug::Dump(buffer, 100);
	//Debug::Dump(buffer, 100);
	//Debug::Print("%i\n", *buffer);

	BMP* bmp = new BMP(buffer);
	Debug::Print("\n%i\n", bmp->width);
	Debug::Print("%i\n", file->locationLBA_LSB);

	for(int y = 0; y < bmp->height; y++)
	{
	for (int x = 0; x < bmp->width; x++)
	{
	uint32 pixel = bmp->pixels[y * bmp->width + x];
	VBE::SetPixel(x, y, pixel);

	//if (pixel == 0xFFFFFFFF)
	//{
	//	Debug::Print("%i\t%i\t%i", x, y, y * bmp->width + x);
	//	while (1);
	//}
	}
	}
	Drawing::Paint();

	Debug::Print("Done\n");
	Debug::Print("%i\n", bmp->pixels[bmp->pixel_count - 1]);
	while (1);*/

	/*std::List<std::String> list;
	std::String str = "a";
	str.Split(list, '#');

	Debug::Print("%i\n\n", list.Count());

	for (int i = 0; i < list.Count(); i++)
	{
		Debug::Print("%i\t%s\n", i, list[i].ToChar());


		//std::List<std::String> nl;
		//list[i].Split(nl, ' ');
		//for (int x = 0; x < nl.Count(); x++)
		//{
		//	Debug::Print("---%i\t%s\n", i, nl[x].ToChar());
		//}
	}

	while (1);*/

	//for (int i = 0; i < 100; i++)
	//{
	//	std::List<std::String> strings;
	//
	//	for (int j = 0; j < 10; j++)
	//	{
	//		strings.Add("hesdf");
	//	}
	//}
	//while (1);
}

void OS::ThrowException(char* error, char* msg)
{
	Panic::KernelPanic(error, msg);
}
