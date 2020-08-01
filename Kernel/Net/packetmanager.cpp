#include <Net/packetmanager.h>
#include <sync.h>
#include <hal.h>

namespace PacketManager
{
    NetInterface *interface;
    Event receive_event;
    Mutex send_mutex;
    int ready_count = 0;

    int Send(NetPacket *pkt)
    {
        if (!pkt)
            return -1;

        send_mutex.Aquire();
        interface->Send(pkt);
        send_mutex.Release();
        return pkt->length;
    }

    void SetInterface(NetInterface *intf)
    {
        interface = intf;
    }

    NetInterface *GetInterface(uint32 ip)
    {
        return interface;
    }

    void NotifyReceived()
    {
        ready_count += 1;
        receive_event.Set();
    }

    void Start()
    {
        while (true)
        {
            receive_event.Wait();
            interface->Poll();

            disable();

            if (--ready_count == 0)
                receive_event.Clear();

            enable();
        }
    }
} // namespace PacketManager
