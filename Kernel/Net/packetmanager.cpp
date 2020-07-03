#include "packetmanager.h"
#include "sync.h"
#include "Process/scheduler.h"

namespace PacketManager
{
    NetInterface *interface;
    Event receive_event;
    Mutex send_mutex;
    int ready_count = 0;

    void Send(NetInterface *intf, NetPacket *pkt)
    {
        send_mutex.Aquire();
        intf->Send(pkt);
        send_mutex.Release();
    }

    void NotifyReceived()
    {
        ready_count += 1;
        receive_event.Set();
    }

    void SetInterface(NetInterface *intf)
    {
        interface = intf;
    }

    void Start()
    {
        while (true)
        {
            receive_event.Wait();
            interface->Poll();

            Scheduler::Disable();

            if (--ready_count == 0)
                receive_event.Clear();

            Scheduler::Enable();
        }
    }
} // namespace PacketManager
