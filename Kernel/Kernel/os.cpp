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
#include "Drivers/serial.h"
#include "Drivers/keyboard.h"
#include "FS/vfs.h"
#include "API/syscalls.h"
#include "Include/syscall_list.h"
#include "FS/iso.h"
#include "Lib/debug.h"
#include "Process/process.h"

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
		debug_print("%c", c);
	}
}

void COM()
{
	if (!Serial::Init(COM_PORT1, 9600))
		debug_print("COM init failed\n");

	debug_print("COM initialized\n");

	THREAD* t = Scheduler::CreateKernelThread(COM_Receive);
	Scheduler::InsertThread(t);

	while (1)
	{
		/*KEY_PACKET key = Keyboard::GetLastKey();

		if (key.key != KEY_INVALID)
		{
			char c = key.character;
			Keyboard::DiscardLastKey();
			Serial::Transmit(COM_PORT1, c);
		}*/
	}
}

void File()
{
	char* buf;
	int size = VFS::ReadFile("files/bunny.obj", buf);

	if (size)
	{
		for (int i = 0; i < size; i++)
			debug_print("%c", buf[i]);
	}
	else
	{
		debug_print("File not found");
	}

	while (1);
}

void OS::Main()
{
	PROCESS* proc = ProcessManager::Load("1winman");
	PIT::Sleep(100);
	ProcessManager::Load("1term");
	ProcessManager::Load("1test");
	ProcessManager::Load("1mndlbrt");

	while (1)
	{
		PROCESS* proc = ProcessManager::GetFirst();

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