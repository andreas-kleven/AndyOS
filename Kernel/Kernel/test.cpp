#include <Drivers/ac97.h>
#include <Drivers/ata.h>
#include <Drivers/e1000.h>
#include <Drivers/keyboard.h>
#include <Drivers/rtc.h>
#include <Drivers/serial.h>
#include <Drivers/tty.h>
#include <Drivers/vt100.h>
#include <Drivers/vtty.h>
#include <FS/devfs.h>
#include <FS/ext2.h>
#include <FS/iso.h>
#include <FS/pipefs.h>
#include <FS/sockfs.h>
#include <FS/vfs.h>
#include <Kernel/dpc.h>
#include <Kernel/task.h>
#include <Kernel/test.h>
#include <Kernel/timer.h>
#include <Net/arp.h>
#include <Net/loopback.h>
#include <Net/packetmanager.h>
#include <Net/socketmanager.h>
#include <Net/tcpsession.h>
#include <Net/udp.h>
#include <Process/dispatcher.h>
#include <Process/process.h>
#include <Process/scheduler.h>
#include <debug.h>
#include <libgen.h>
#include <math.h>
#include <net.h>
#include <panic.h>
#include <pci.h>
#include <queue.h>
#include <stdio.h>
#include <sync.h>
#include <syscall_list.h>

namespace Test {
void COM_Receive()
{
    while (1) {
        char c = Serial::Receive(COM_PORT1);
        kprintf("%c", c);
    }
}

void COM()
{
    if (!Serial::Init(COM_PORT1, 9600))
        kprintf("COM init failed\n");

    kprintf("COM initialized\n");

    THREAD *t = Task::CreateKernelThread(COM_Receive);
    Scheduler::InsertThread(t);

    while (1) {
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
    Scheduler::Disable();
    PROCESS *proc1 = ProcessManager::Exec("/1winman");
    PROCESS *proc2 = ProcessManager::Exec("/1term");
    // PROCESS *proc3 = ProcessManager::Exec("/1info");
    Scheduler::Enable();

    /*Scheduler::SleepThread(Timer::Ticks() + 1000000, Scheduler::CurrentThread());
    Scheduler::Disable();
    PROCESS *proc4 = ProcessManager::Exec("/1info");
    Scheduler::Enable();*/

    // ProcessManager::Exec("/1test");
    // ProcessManager::Exec("/1mndlbrt");
}

void TTY()
{
    THREAD *vtty_thread = Task::CreateKernelThread(VTTY::Start);
    Scheduler::InsertThread(vtty_thread);

    Scheduler::SleepThread(Timer::Ticks() + 100000, Scheduler::CurrentThread());
    Scheduler::Disable();

    for (int i = 0; i < 8; i++) {
        char name[32];
        sprintf(name, "/dev/tty%d", i);
        DENTRY *dentry = VFS::GetDentry(name);

        if (!dentry) {
            kprintf("tty not found '%s'\n", name);
            continue;
        }

        TtyDriver *tty = (TtyDriver *)DriverManager::GetDriver(dentry->inode->dev);

        const char *argv[] = {0};
        const char *envp[] = {"COLUMNS=80", "LINES=48", 0};
        PROCESS *proc = ProcessManager::Exec("/bin/dash", argv, envp);

        tty->gid = proc->gid;
        tty->sid = proc->id;
        proc->sid = proc->id;

        int fd = VFS::Open(proc, name, 0);
        VFS::DuplicateFile(proc->filetable, fd, 0);
        VFS::DuplicateFile(proc->filetable, fd, 1);
        VFS::DuplicateFile(proc->filetable, fd, 2);
        kprintf("Fd %d\n", fd);
    }

    Scheduler::Enable();
}

void InitNet()
{
    Net::Init();

    PciDevice *pci_dev = PCI::GetDevice(2, 0, 0);

    if (!pci_dev) {
        kprintf("Network card not found\n");
        return;
    }

    kprintf("Found network card\n");

    LoopbackInterface *loopback = new LoopbackInterface();
    PacketManager::RegisterInterface(loopback, true);

    E1000 *intf = new E1000(pci_dev);
    PacketManager::RegisterInterface(intf, false);
}

void _Net()
{
    char buf[1024];

    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(10001);

    // TcpSession *session = TCP::CreateSession();
    Socket *server = SocketManager::CreateSocket();
    server->Init(AF_INET, SOCK_STREAM, 0);

    srand(Timer::Ticks() + RTC::Minute() * 1000 + RTC::Second());

    server->Bind((sockaddr *)&servaddr, sizeof(sockaddr_in));
    server->Listen(10);
    kprintf("%p %p %d\n", server, server->tcp_session->socket, server->listening);

    while (true) {
        int id = server->Accept((sockaddr *)&servaddr, sizeof(sockaddr_in), 0);
        Socket *client = SocketManager::GetSocket(id);
        kprintf("Client: %p\n", client);

        if (client) {
            sprintf(buf, "Hello tcp %d\n", (int)Timer::Ticks() / 1000);
            client->Send(buf, strlen(buf), 0);

            char recvbuf[1024];
            int recv = client->Recv(recvbuf, sizeof(recvbuf), 0);
            debug_dump(recvbuf, recv, true);

            client->Shutdown(0);
        }
    }

    // Scheduler::SleepThread(Timer::Ticks() + 1000000, Scheduler::CurrentThread());
    // session->Close();

    // session->Listen(10001);
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
    kprintf("Free: %i\n", PMem::NumFree());

    int *ptr1 = (int *)VMem::KernelAlloc(1);
    kprintf("Free: %i\t Addr: %p\n", PMem::NumFree(), ptr1);

    int *ptr2 = (int *)VMem::KernelAlloc(1);
    kprintf("Free: %i\t Addr: %p\n", PMem::NumFree(), ptr2);

    int *ptr3 = (int *)VMem::KernelAlloc(1);
    kprintf("Free: %i\t Addr: %p\n", PMem::NumFree(), ptr3);

    VMem::FreePages(ptr2, 2);
    kprintf("Free: %i\n", PMem::NumFree());

    int *_ptr1 = (int *)VMem::KernelAlloc(2);
    kprintf("Free: %i\t Addr: %p\n", PMem::NumFree(), _ptr1);
}

Mutex mutex;
int count = 0;

void MutexFunc(int i)
{
    mutex.Aquire();

    if (i == 1)
        mutex.Aquire();

    count += 1;
    kprintf("Before %i, %i\n", i, count);
    Scheduler::SleepThread(Timer::Ticks() + 100000, Scheduler::CurrentThread());
    kprintf("After %i, %i\n", i, count);

    if (i == 1) {
        mutex.Release();

        count += 1;
        kprintf("Before %i, %i\n", i, count);
        Scheduler::SleepThread(Timer::Ticks() + 100000, Scheduler::CurrentThread());
        kprintf("After %i, %i\n", i, count);
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

    if (!driver1 || VFS::Mount(driver1, fs1, "/"))
        kprintf("Mount 1 failed\n");

    if (!driver2 || VFS::Mount(driver2, fs2, "/mnt"))
        kprintf("Mount 2 failed\n");

    if (VFS::Mount(0, devfs, "/dev"))
        kprintf("Mount devfs failed\n");

    if (VFS::Mount(0, pipefs, "/pipe"))
        kprintf("Mount pipefs failed\n");

    if (VFS::Mount(0, sockfs, "/sock"))
        kprintf("Mount sockfs failed\n");
}

void Start()
{
    THREAD *dpc_thread = Task::CreateKernelThread(Dpc::Start);
    Scheduler::InsertThread(dpc_thread);

    THREAD *dispatcher_thread = Task::CreateKernelThread(Dispatcher::Start);
    Scheduler::InsertThread(dispatcher_thread);

    Mount();
    InitNet();
    // GUI();
    TTY();
    //_Memory();
    //_Net();
    // COM();
    // MutexTest();

    Scheduler::ExitThread(0, Scheduler::CurrentThread());
    Scheduler::Switch();
}
} // namespace Test
