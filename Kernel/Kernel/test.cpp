#include "test.h"
#include "panic.h"
#include "stdio.h"
#include "PCI/pci.h"
#include "Drivers/ata.h"
#include "Drivers/ac97.h"
#include "Drivers/e1000.h"
#include "Net/arp.h"
#include "Net/net.h"
#include "Net/socketmanager.h"
#include "Net/tcpsocket.h"
#include "Net/udp.h"
#include "Net/dns.h"
#include "Net/dhcp.h"
#include "Net/packetmanager.h"
#include "Process/scheduler.h"
#include "Drivers/serial.h"
#include "Drivers/keyboard.h"
#include "FS/vfs.h"
#include "FS/devfs.h"
#include "FS/pipefs.h"
#include "FS/sockfs.h"
#include "FS/iso.h"
#include "FS/ext2.h"
#include "Process/process.h"
#include "Process/dispatcher.h"
#include "sync.h"
#include "task.h"
#include "timer.h"
#include "syscall_list.h"
#include "debug.h"

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

	void File()
	{
		Filetable filetable;

		int fd1 = VFS::Open(filetable, "/test");
		int fd2 = VFS::Open(filetable, "/dev/mouse");

		debug_print("Open: %d %d\n", fd1, fd2);
		char buf[100];

		if (fd1 >= 0)
		{
			memset(buf, 0, sizeof(buf));
			VFS::Read(filetable, fd1, buf, sizeof(buf));
			debug_dump(buf, sizeof(buf), true);
		}

		if (fd2 >= 0)
		{
			memset(buf, 0, sizeof(buf));
			VFS::Read(filetable, fd2, buf, sizeof(buf));
			debug_dump(buf, sizeof(buf), false);
		}
	}

	void GUI()
	{
		THREAD *dispatcher_thread = Task::CreateKernelThread(Dispatcher::Start);
		Scheduler::InsertThread(dispatcher_thread);

		Scheduler::Disable();
		PROCESS *proc1 = ProcessManager::Exec("1winman");
		PROCESS *proc2 = ProcessManager::Exec("1term");
		//PROCESS *proc3 = ProcessManager::Exec("1info");
		Scheduler::Enable();

		//ProcessManager::Exec("1test");
		//ProcessManager::Exec("1mndlbrt");

#ifdef __i386__
#include "Arch/regs.h"
		while (1)
		{
			Scheduler::SleepThread(Timer::Ticks() + 100000, Scheduler::CurrentThread());
			PROCESS *proc = ProcessManager::GetFirst();

			while (proc)
			{
				if (!proc->messages.IsEmpty())
				{
					MESSAGE *msg = proc->messages.Get();

					if (msg)
					{
						disable();
						VMem::SwapAddressSpace(proc->addr_space);

						if (proc->message_handler)
						{
							THREAD *thread = ProcessManager::CreateThread(proc, (void (*)())proc->message_handler);

							REGS *regs = (REGS *)thread->stack;
							char *data_ptr = (char *)regs->user_stack - msg->size - 4;
							memcpy(data_ptr, msg->data, msg->size);

							uint32 *stack_ptr = (uint32 *)data_ptr;
							*--stack_ptr = msg->size;
							*--stack_ptr = (int)data_ptr;
							*--stack_ptr = msg->param;
							*--stack_ptr = msg->src_proc;
							*--stack_ptr = msg->id;
							regs->user_stack -= 28 + msg->size;

							Scheduler::InsertThread(thread);

							proc->messages.Pop();
						}

						enable();
					}
				}

				proc = proc->next;
			}
		}
#endif
	}

	void InitNet()
	{
		Net::Init();

		PciDevice *dev = PCI::GetDevice(2, 0, 0);

		if (!dev)
		{
			debug_print("Network card not found\n");
			return;
		}

		debug_print("Found network card\n");

		E1000 *intf = new E1000(dev);

		THREAD *packet_thread = Task::CreateKernelThread(PacketManager::Start);
		Scheduler::InsertThread(packet_thread);
		PacketManager::SetInterface(intf);
	}

	void _Net()
	{
		sockaddr_in servaddr;
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = INADDR_ANY;
		servaddr.sin_port = htons(8080);

		sockaddr_in clientaddr;
		clientaddr.sin_family = AF_INET;
		clientaddr.sin_addr.s_addr = htonl(0xC0A8007B); // 192.168.0.123
		clientaddr.sin_port = htons(8080);

		Socket *socket = SocketManager::CreateSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		socket->Bind((sockaddr *)&servaddr, sizeof(servaddr));

		char buf[1024];

		while (true)
		{
			int len;

			//len = socket->Recv(buf, sizeof(buf), 0);
			//debug_dump(buf, len, true);
			//continue;

			sprintf(buf, "Hello %d\n", (int)Timer::Ticks() / 1000);
			len = strlen(buf);
			socket->Sendto(buf, len, 0, (sockaddr *)&clientaddr, sizeof(clientaddr));

			Scheduler::SleepThread(Timer::Ticks() + 1000000, Scheduler::CurrentThread());
		}
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
		{
			debug_print("Mount 1 failed\n");
			return;
		}

		if (VFS::Mount(driver2, fs2, "/mnt"))
		{
			debug_print("Mount 2 failed\n");
			return;
		}

		if (VFS::Mount(0, devfs, "/dev"))
		{
			debug_print("Mount devfs failed\n");
			return;
		}

		if (VFS::Mount(0, pipefs, "/pipe"))
		{
			debug_print("Mount pipefs failed\n");
			return;
		}

		if (VFS::Mount(0, sockfs, "/sock"))
		{
			debug_print("Mount sockfs failed\n");
			return;
		}
	}

	void Start()
	{
		Mount();
		InitNet();
		GUI();
		//_Memory();
		//File();
		//_Net();
		//Audio();
		//COM();
		//MutexTest();

		while (1)
			pause();
	}
} // namespace Test
