#pragma once
#include "packet.h"
#include "netinterface.h"
#include "socket.h"

namespace PacketManager
{
    int Send(NetPacket *pkt);
    void SetInterface(NetInterface *intf);
    NetInterface *GetInterface(uint32 ip);
    void NotifyReceived();
    void Start();
} // namespace PacketManager
