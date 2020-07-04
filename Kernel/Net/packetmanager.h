#pragma once
#include "packet.h"
#include "netinterface.h"
#include "socket.h"

namespace PacketManager
{
    int Send(NetPacket *pkt);
    void SetInterface(NetInterface *intf);
    NetInterface *GetInterface(const sockaddr *addr);
    NetInterface *GetInterface(const sockaddr_in *addr);
    void NotifyReceived();
    void Start();
} // namespace PacketManager
