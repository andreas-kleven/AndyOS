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
#include "Drivers/serial.h"
#include "Drivers/keyboard.h"
#include "FS/vfs.h"
#include "API/syscalls.h"
#include "Include/syscall_list.h"
#include "FS/iso.h"
#include "Lib/debug.h"
#include "Process/process.h"
#include "Drawing/drawing.h"

void _Process()
{
	ProcessManager::Load("Test.exe");
	ProcessManager::Load("_Test.exe");
}

void T1()
{
	const char* text = "'";

	while (1)
	{
		asm("mov %0, %%eax\n"
			"mov %1, %%ebx\n"
			"int $0x80\n" 
			"pause"
			:: "c" (SYSCALL_PRINT), "d" (text));
	}
}

void T2()
{
	uint32 colors[] =
	{
		0xFFFF0000,
		0xFF00FF00,
		0xFF0000FF,
		0xFF00FFFF,
		0xFFFF00FF,
		0xFFFFFF00
	};

	int t = 0;

	while (1)
	{
		uint32 color = colors[(t++ / 10) % 6];

		asm("mov %0, %%eax\n"
			"mov %1, %%ebx\n"
			"int $0x80\n" 
			"pause"
			:: "c" (SYSCALL_COLOR), "d" (color));
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
		if (arg2)
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
		else
		{
			Debug::Print("%s\n", terminal_path);
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

		if (size == 0)
		{
			Debug::Print("File not found\n");
		}
		else
		{
			Debug::Dump(buf, size);

			for (int i = 0; i < size; i++)
			{
				Debug::Putc(buf[i]);
			}
		}
	}
	else if (strcmp(arg1, "open") == 0)
	{
		ProcessManager::Load(arg2);
	}
	else if (strcmp(arg1, "gui") == 0)
	{
		ProcessManager::Load("winman");
	}
	else if (strcmp(arg1, "color") == 0)
	{
		Debug::color = (0xFF << 24) | strtol(arg2, 0, 16);
	}
	else if (strcmp(arg1, "clear") == 0)
	{
		Debug::Clear(Debug::color);
		Drawing::Clear(0xFF000000, Drawing::gc);
	}
	else if (strcmp(arg1, "sqrt") == 0)
	{
		float f = atof(arg2);
		Debug::Print("%f\t%f\n", f, tan(f));
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

		if (pack.key != KEY_INVALID)
		{
			Keyboard::DiscardLastKey();

			if(pack.pressed)
			{
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
	PROCESS* proc = ProcessManager::Load("1winman");
	ProcessManager::Load("1game");
	PROCESS* last = ProcessManager::Load("1test");

	while (1)
	{
		PROCESS* proc = last;

		while (proc)
		{
			if (!proc->messages.IsEmpty())
			{
				MESSAGE* msg = proc->messages.Get();

				if (msg)
				{
					asm("cli");
					VMem::SwitchDir(proc->page_dir);

					if (msg->type == MESSAGE_TYPE_SIGNAL)
					{
						if (proc->signal_handler)
						{
							THREAD* thread = ProcessManager::CreateThread(proc, (void(*)())proc->signal_handler);

							uint32* stack_ptr = (uint32*)thread->regs->user_stack;
							*--stack_ptr = msg->param;
							thread->regs->user_stack -= 8;
							
							Scheduler::InsertThread(thread);
						}
					}
					else
					{
						if (proc->message_handler)
						{
							THREAD* thread = ProcessManager::CreateThread(proc, (void(*)())proc->message_handler);

							char* data_ptr = (char*)thread->regs->user_stack - msg->size - 4;
							memcpy(data_ptr, msg->data, msg->size);

							uint32* stack_ptr = (uint32*)data_ptr;
							*--stack_ptr = msg->size;
							*--stack_ptr = (int)data_ptr;
							*--stack_ptr = msg->param;
							*--stack_ptr = msg->src_proc;
							*--stack_ptr = msg->id;
							thread->regs->user_stack -= 28 + msg->size;

							Scheduler::InsertThread(thread);
						}
					}

					asm("sti");
				}
			}

			proc = proc->next;
		}
	}

	/*ProcessManager::Load("1game");

	while (1)
	{
		asm("int %0" :: "N" (TASK_SCHEDULE_IRQ));
	}*/

	Terminal();
	//File();

	char* _s1 = (char*)VMem::UserAlloc(1);
	char* _s2 = (char*)VMem::UserAlloc(1);

	VMem::MapPhysAddr((uint32)T1, (uint32)T1, PTE_PRESENT | PTE_WRITABLE | PTE_USER, 1);
	VMem::MapPhysAddr((uint32)T2, (uint32)T2, PTE_PRESENT | PTE_WRITABLE | PTE_USER, 1);

	//THREAD* t1 = Scheduler::CreateKernelThread(T1);
	//THREAD* t2 = Scheduler::CreateKernelThread(T2);
	THREAD* t1 = Scheduler::CreateUserThread(T1, _s1 + BLOCK_SIZE);
	THREAD* t2 = Scheduler::CreateUserThread(T2, _s2 + BLOCK_SIZE);

	Scheduler::InsertThread(t1);
	Scheduler::InsertThread(t2);
	while (1);

	//Net::Init();

	//Mandelbrot mandelbrot(Drawing::gc);
	//mandelbrot.Run();

	//Game();
	//Audio();
	//_Font();
	//COM();
	_Process();
	while (1);
}