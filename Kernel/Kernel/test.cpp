#include <Kernel/test.h>
#include <panic.h>
#include <stdio.h>
#include <libgen.h>
#include <math.h>
#include <pci.h>
#include <Drivers/ata.h>
#include <Drivers/ac97.h>
#include <Drivers/e1000.h>
#include <Net/arp.h>
#include <net.h>
#include <Net/socketmanager.h>
#include <Net/tcpsession.h>
#include <Net/udp.h>
#include <Net/packetmanager.h>
#include <Process/scheduler.h>
#include <Drivers/serial.h>
#include <Drivers/keyboard.h>
#include <Drivers/rtc.h>
#include <FS/vfs.h>
#include <FS/devfs.h>
#include <FS/pipefs.h>
#include <FS/sockfs.h>
#include <FS/iso.h>
#include <FS/ext2.h>
#include <Process/process.h>
#include <Process/dispatcher.h>
#include <sync.h>
#include <Kernel/task.h>
#include <Kernel/timer.h>
#include <syscall_list.h>
#include <debug.h>

namespace Test
{
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

		THREAD *t = Task::CreateKernelThread(COM_Receive);
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

	void GUI()
	{
		THREAD *dispatcher_thread = Task::CreateKernelThread(Dispatcher::Start);
		Scheduler::InsertThread(dispatcher_thread);

		Scheduler::Disable();
		PROCESS *proc1 = ProcessManager::Exec("/1winman");
		PROCESS *proc2 = ProcessManager::Exec("/1term");
		//PROCESS *proc3 = ProcessManager::Exec("/1info");
		Scheduler::Enable();

		/*Scheduler::SleepThread(Timer::Ticks() + 1000000, Scheduler::CurrentThread());
		Scheduler::Disable();
		PROCESS *proc4 = ProcessManager::Exec("/1info");
		Scheduler::Enable();*/

		//ProcessManager::Exec("/1test");
		//ProcessManager::Exec("/1mndlbrt");

#ifdef __i386__
#include <Arch/regs.h>
		while (1)
		{
			Scheduler::SleepThread(Timer::Ticks() + 100000, Scheduler::CurrentThread());
			PROCESS *proc = ProcessManager::GetFirst();

			while (proc)
			{
				if (!proc->messages.IsEmpty())
				{
					disable();
					MESSAGE *msg = proc->messages.Get();

					if (msg)
					{
						VMem::SwapAddressSpace(proc->addr_space);

						if (proc->message_handler)
						{
							THREAD *thread = ProcessManager::CreateThread(proc, (void (*)())proc->message_handler);

							REGS *regs = (REGS *)thread->stack;
							char *data_ptr = (char *)regs->user_stack - msg->size - 4;
							memcpy(data_ptr, msg->data, msg->size);

							uint32 *stack_ptr = (uint32 *)data_ptr;
							*--stack_ptr = msg->size;
							*--stack_ptr = (uint32)data_ptr;
							*--stack_ptr = msg->param;
							*--stack_ptr = msg->src_proc;
							*--stack_ptr = msg->id;
							regs->user_stack = (uint32)(stack_ptr - 1);

							Scheduler::InsertThread(thread);

							proc->messages.Pop();
						}
					}

					enable();
				}

				proc = proc->next;
			}
		}
#endif
	}

	void InitNet()
	{
		Net::Init();

		PciDevice *pci_dev = PCI::GetDevice(2, 0, 0);

		if (!pci_dev)
		{
			debug_print("Network card not found\n");
			return;
		}

		debug_print("Found network card\n");

		E1000 *intf = new E1000(pci_dev);

		THREAD *packet_thread = Task::CreateKernelThread(PacketManager::Start);
		Scheduler::InsertThread(packet_thread);
		PacketManager::SetInterface(intf);
	}

	void _Net()
	{
		char buf[1024];

		sockaddr_in servaddr;
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = INADDR_ANY;
		servaddr.sin_port = htons(10001);

		sockaddr_in clientaddr;
		clientaddr.sin_family = AF_INET;
		clientaddr.sin_addr.s_addr = htonl(0xC0A8007B); // 192.168.0.123
		clientaddr.sin_port = htons(10001);

		//TcpSession *session = TCP::CreateSession();
		Socket *server = SocketManager::CreateSocket();
		server->Init(AF_INET, SOCK_STREAM, 0);

		srand(Timer::Ticks() + RTC::Minute() * 1000 + RTC::Second());

		//while (true)
		{
			//session->Connect(clientaddr, rand());
			//Scheduler::SleepThread(Timer::Ticks() + 1000000, Scheduler::CurrentThread());
		}

		server->Bind((sockaddr *)&servaddr, sizeof(sockaddr_in));
		server->Listen(10);
		debug_print("%p %p %d\n", server, server->tcp_session->socket, server->listening);

		while (true)
		{
			int id = server->Accept((sockaddr *)&servaddr, sizeof(sockaddr_in), 0);
			Socket *client = SocketManager::GetSocket(id);
			debug_print("Client: %p\n", client);

			if (client)
			{
				sprintf(buf, "Hello tcp %d\n", (int)Timer::Ticks() / 1000);
				client->Send(buf, strlen(buf), 0);

				char recvbuf[1024];
				int recv = client->Recv(recvbuf, sizeof(recvbuf), 0);
				debug_dump(recvbuf, recv, true);

				client->Shutdown(0);
			}
		}

		//Scheduler::SleepThread(Timer::Ticks() + 1000000, Scheduler::CurrentThread());
		//session->Close();

		//session->Listen(10001);
		return;

		/*Socket *socket = SocketManager::CreateSocket();
		socket->init(AF_INET, SOCK_DGRAM, 0);
		socket->Bind((sockaddr *)&servaddr, sizeof(servaddr));

		while (true)
		{
			int len;

			//len = socket->Recv(buf, sizeof(buf), 0);
			//debug_dump(buf, len, true);
			//continue;

			sprintf(buf, "Hello %d\n", (int)Timer::Ticks() / 1000);
			socket->Sendto(buf, strlen(buf), 0, (sockaddr *)&clientaddr, sizeof(clientaddr));

			Scheduler::SleepThread(Timer::Ticks() + 1000000, Scheduler::CurrentThread());
		}*/
	}

	void _Memory()
	{
		debug_print("Free: %i\n", PMem::NumFree());

		int *ptr1 = (int *)VMem::KernelAlloc(1);
		debug_print("Free: %i\t Addr: %p\n", PMem::NumFree(), ptr1);

		int *ptr2 = (int *)VMem::KernelAlloc(1);
		debug_print("Free: %i\t Addr: %p\n", PMem::NumFree(), ptr2);

		int *ptr3 = (int *)VMem::KernelAlloc(1);
		debug_print("Free: %i\t Addr: %p\n", PMem::NumFree(), ptr3);

		VMem::FreePages(ptr2, 2);
		debug_print("Free: %i\n", PMem::NumFree());

		int *_ptr1 = (int *)VMem::KernelAlloc(2);
		debug_print("Free: %i\t Addr: %p\n", PMem::NumFree(), _ptr1);
	}

	Mutex mutex;
	int count = 0;

	void MutexFunc(int i)
	{
		mutex.Aquire();

		if (i == 1)
			mutex.Aquire();

		count += 1;
		debug_print("Before %i, %i\n", i, count);
		Scheduler::SleepThread(Timer::Ticks() + 100000, Scheduler::CurrentThread());
		debug_print("After %i, %i\n", i, count);

		if (i == 1)
		{
			mutex.Release();

			count += 1;
			debug_print("Before %i, %i\n", i, count);
			Scheduler::SleepThread(Timer::Ticks() + 100000, Scheduler::CurrentThread());
			debug_print("After %i, %i\n", i, count);
		}

		mutex.Release();

		while (1)
			pause();
	}

	void MutexFunc1()
	{
		MutexFunc(1);
	}

	void MutexFunc2()
	{
		MutexFunc(2);
	}

	void MutexTest()
	{
		mutex = Mutex();

		THREAD *t1 = Task::CreateKernelThread(MutexFunc1);
		THREAD *t2 = Task::CreateKernelThread(MutexFunc2);

		Scheduler::Disable();
		Scheduler::InsertThread(t1);
		Scheduler::InsertThread(t2);
		Scheduler::Enable();
	}

	void Mount()
	{
		ATADriver *driver1 = (ATADriver *)DriverManager::GetDriver("hdc");
		IsoFS *fs1 = new IsoFS();
		ATADriver *driver2 = (ATADriver *)DriverManager::GetDriver("hda");
		Ext2FS *fs2 = new Ext2FS();

		DevFS *devfs = new DevFS();
		PipeFS *pipefs = new PipeFS();
		SockFS *sockfs = new SockFS();

		if (VFS::Mount(driver1, fs1, "/"))
			debug_print("Mount 1 failed\n");

		if (VFS::Mount(driver2, fs2, "/mnt"))
			debug_print("Mount 2 failed\n");

		if (VFS::Mount(0, devfs, "/dev"))
			debug_print("Mount devfs failed\n");

		if (VFS::Mount(0, pipefs, "/pipe"))
			debug_print("Mount pipefs failed\n");

		if (VFS::Mount(0, sockfs, "/sock"))
			debug_print("Mount sockfs failed\n");
	}

	void Start()
	{
		Mount();
		//InitNet();
		GUI();
		//_Memory();
		//_Net();
		//Audio();
		//COM();
		//MutexTest();

		while (1)
			pause();
	}
} // namespace Test
