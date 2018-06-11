#include "os.h"
#include "panic.h"
#include "stdio.h"
#include "Drivers/ac97.h"
#include "Net/net.h"
#include "Net/tcpsocket.h"
#include "Net/udpsocket.h"
#include "Net/dns.h"
#include "Net/dhcp.h"
#include "Net/http.h"
#include "Process/scheduler.h"
#include "Drawing/font.h"
#include "GUI/GUI.h"
#include "System.h"
#include "Drivers/serial.h"
#include "Test/Mandelbrot.h"
#include "Test/TextEdit.h"
#include "API/syscalls.h"
#include "FS/iso.h"

#include "debug.h"

#include "../3DGame/3DGame.h"


void GUI()
{
	gui::WindowManager::Init();

	Scheduler::InsertThread(Scheduler::CreateKernelThread(apps::RunTextEdit));
	Scheduler::InsertThread(Scheduler::CreateKernelThread(apps::RunTextEdit));

	//Task::InsertThread(Task::CreateThread(gui::WindowManager::Start));
	gui::WindowManager::Start();
}

GC gameGC;

void render()
{
	while (1)
	{
		Drawing::BitBlt(gameGC, 0, 0, gameGC.width, gameGC.height, Drawing::gc, 0, 0);
	}
}

void Game()
{
	gameGC = Drawing::gc;
	//gameGC = GC(Drawing::gc, 100, 100, 800, 600);

	//gameGC = GC::CreateGraphics(500, 500);
	//Task::InsertThread(Task::CreateThread(render));

	while (1)
	{
		GEngine* engine = new GEngine(gameGC);
		MyGame* game = new MyGame();
		engine->StartGame(game);
	}
}

void Audio()
{
	PCI_DEVICE* dev = PCI::GetDevice(4, 1, 0);

	if (dev)
	{
		Debug::Print("Found audio device\n");
		AC97::Init(dev);
	}
	else
	{
		Debug::Print("Audio device not found!\n");
	}

	while (1);
}

void _Font()
{
	char* name = "arial";
	//name = "calibri";
	name = "consolas";

	Font font(name);
	while (1);
}

#include "Process/process.h"
void _Process()
{
	Process::Create("Test.exe");
	Process::Create("_Test.exe");
}

void T1()
{
	const char* text = "'";

	while (1)
	{
		_asm
		{
			mov eax, SYSCALL_PRINT
			mov ebx, text
			int 0x80
			pause
		}
	}
}

void T2()
{
	uint32 colors[] =
	{
		COLOR_RED,
		COLOR_BLUE,
		COLOR_GREEN,
		COLOR_CYAN,
		COLOR_MAGENTA,
		COLOR_YELLOW
	};

	int t = 0;

	while (1)
	{
		//Debug::color = colors[(t++ / 10) % 6];
		//Debug::bcolor = 0;

		uint32 color = colors[(t++ / 10) % 6];

		_asm
		{
			mov eax, SYSCALL_COLOR
			mov ebx, color
			int 0x80
			pause

			mov eax, SYSCALL_SLEEP
			mov ebx, 10
			int 0x80
		}
	}
}

void COM_Receive()
{
	while (1)
	{
		char c = Serial::Receive(COM_PORT1);
		Debug::Print("%c", c);
	}
}

void COM()
{
	if (!Serial::Init(COM_PORT1, 9600))
		Debug::Print("COM init failed\n");

	Debug::Print("COM initialized\n");

	THREAD* t = Scheduler::CreateKernelThread(COM_Receive);
	Scheduler::InsertThread(t);

	while (1)
	{
		KEY_PACKET key = Keyboard::GetLastKey();

		if (key.key != KEY_INVALID)
		{
			char c = key.character;
			Keyboard::DiscardLastKey();
			Serial::Transmit(COM_PORT1, c);
		}
	}
}

void File()
{
	char* buf;
	int size = VFS::ReadFile("files/bunny.obj", buf);

	if (size)
	{
		for (int i = 0; i < size; i++)
			Debug::Print("%c", buf[i]);
	}
	else
	{
		Debug::Print("File not found");
	}

	while (1);
}

char terminal_path[256] = "/";

void HandleCommand(char* cmd)
{
	char* saveptr;

	char* arg1 = 0;
	char* arg2 = 0;
	char* arg3 = 0;

	arg1 = strtok_r(cmd, " \t", &saveptr);
	if (arg1) arg2 = strtok_r(0, " \t", &saveptr);
	if (arg2) arg3 = strtok_r(0, " \t", &saveptr);

	if (strcmp(arg1, "ls") == 0)
	{
		FILE_INFO* files;
		DIRECTORY_INFO* dirs;
		int file_count;
		int dir_count;

		VFS::List(terminal_path, files, dirs, file_count, dir_count);

		for (int i = 0; i < file_count; i++)
		{
			Debug::Print("%s\n", files[i].name);
		}

		for (int i = 0; i < dir_count; i++)
		{
			Debug::Print("%s\n", dirs[i].name);
		}
	}
	else if (strcmp(arg1, "cd") == 0)
	{
		if (strcmp(arg2, "..") == 0)
		{
			int length = strlen(terminal_path) - 1;

			if (length > 0)
			{
				while (terminal_path[--length] != '/')
					terminal_path[length] = 0;
			}
		}
		else
		{
			int arg2_len = strlen(arg2);

			if (arg2_len > 0 && arg2[0] != '/')
			{
				strcpy(terminal_path + strlen(terminal_path), arg2);

				int length = strlen(terminal_path);
				if (terminal_path[length - 1] != '/')
				{
					terminal_path[length] = '/';
					terminal_path[length + 1] = 0;
				}
			}
		}
	}
	else if (strcmp(arg1, "read") == 0)
	{
		int path_len = strlen(terminal_path) + strlen(arg2);
		char* path = new char[path_len];
		memset(path, 0, path_len);

		strcpy(path, terminal_path);
		strcpy(path + strlen(terminal_path), arg2);

		char* buf;
		uint32 size = VFS::ReadFile(path, buf);

		Debug::Dump(buf, size);

		for (int i = 0; i < size; i++)
		{
			Debug::Putc(buf[i]);
		}
	}
	else
	{
		Debug::Print("Invalid command\n");
	}
}

void GetCommand(char* buf)
{
	int index = 0;
	buf[0] = 0;

	while (1)
	{
		KEY_PACKET pack = Keyboard::GetLastKey();

		if (pack.key != KEY_INVALID && pack.pressed)
		{
			Keyboard::DiscardLastKey();

			if (pack.key == KEY_RETURN)
			{
				return;
			}
			else if (pack.key == KEY_BACK)
			{
				buf[index] = 0;

				if (index > 0)
				{
					index--;
					Debug::Putc('\b');
				}
			}
			else
			{
				if (pack.character)
				{
					buf[index++] = pack.character;
					buf[index] = 0;
					Debug::Putc(pack.character);
				}
			}
		}
	}
}

void Terminal()
{
	int index;
	char buf[256];

	while (1)
	{
		Debug::Print("%s>", terminal_path);
		GetCommand(buf);
		Debug::Print("\n");
		HandleCommand(buf);
		Debug::Print("\n");
	}
}

void OS::Main()
{
	//File();
	//Terminal();

	char* _s1 = (char*)VMem::UserAlloc(VMem::GetCurrentDir(), 1);
	char* _s2 = (char*)VMem::UserAlloc(VMem::GetCurrentDir(), 1);

	VMem::MapPhysAddr(VMem::GetCurrentDir(), (uint32)T1, (uint32)T1, PTE_PRESENT | PTE_WRITABLE | PTE_USER, 1);
	VMem::MapPhysAddr(VMem::GetCurrentDir(), (uint32)T2, (uint32)T2, PTE_PRESENT | PTE_WRITABLE | PTE_USER, 1);
	//File();

	//THREAD* t1 = Scheduler::CreateKernelThread(T1);
	//THREAD* t2 = Scheduler::CreateKernelThread(T2);
	THREAD* t1 = Scheduler::CreateUserThread(T1, _s1 + BLOCK_SIZE);
	THREAD* t2 = Scheduler::CreateUserThread(T2, _s2 + BLOCK_SIZE);
	
	Scheduler::InsertThread(t2);
	Scheduler::InsertThread(t1);
	//while (1);

	//Net::Init();

	//Mandelbrot mandelbrot(Drawing::gc);
	//mandelbrot.Run();

	//GUI();
	//Game();
	//Audio();
	//_Font();
	//COM();
	_Process();
	while (1);

	PIT::Sleep(2000);
	GUI();

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

	UdpSocket* us = UDP::CreateSocket(1881);

	Debug::Print("Ready\n");


	while (1)
	{
		IPv4Address recA;
		uint8* recB;
		int length = us->Receive(recB, recA);
		recB[length] = 0;

		Debug::Print("%s\n", recB);

		/*while (Keyboard::GetLastKey().key == KEY_INVALID || !Keyboard::GetLastKey().pressed);
		KEY_PACKET key = Keyboard::GetLastKey();
		Keyboard::DiscardLastKey();

		char* data = "hello ..";
		us->Send(addr, (uint8*)data, strlen(data));

		Debug::Print("Sent\n");*/
	}


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

		Net::PrintIP("andyhk.ga: ", DNS::LookupAddress("andyhk.ga"));

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
}