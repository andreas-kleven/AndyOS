#include <Net/packetmanager.h>
#include <hal.h>
#include <list.h>
#include <sync.h>

namespace PacketManager {
List<NetInterface *> interfaces;
Event receive_event;
Mutex send_mutex;
int ready_count = 0;
NetInterface *loopback_interface;

void RegisterInterface(NetInterface *intf, bool loopback)
{
    if (loopback)
        loopback_interface = intf;

    if (!intf)
        return;

    interfaces.Add(intf);
}

NetInterface *GetInterface(uint32 ip)
{
    for (size_t i = 0; i < interfaces.Count(); i++) {
        NetInterface *intf = interfaces[i];
        uint32 mask = intf->GetMask();

        if (loopback_interface && ip == intf->GetIP())
            return loopback_interface;

        if ((intf->GetIP() & mask) == (ip & mask))
            return intf;
    }

    panic("No network interface found", "IP: %p", ip);
    return 0;
}

void Poll(NetInterface *intf)
{
    intf->Poll();
}

int Send(NetPacket *pkt)
{
    if (!pkt || !pkt->interface)
        return -1;

    send_mutex.Aquire();
    pkt->interface->Send(pkt);
    send_mutex.Release();
    return pkt->length;
}
} // namespace PacketManager
