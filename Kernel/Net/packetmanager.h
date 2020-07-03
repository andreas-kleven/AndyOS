#pragma once
#include "packet.h"
#include "netinterface.h"

namespace PacketManager
{
    void Send(NetInterface *intf, NetPacket *pkt);
    void NotifyReceived();
    void SetInterface(NetInterface *intf);
    void Start();
} // namespace PacketManager
