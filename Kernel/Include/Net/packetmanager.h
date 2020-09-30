#pragma once
#include <Net/netinterface.h>
#include <Net/packet.h>
#include <Net/socket.h>

namespace PacketManager {
void RegisterInterface(NetInterface *intf, bool loopback);
NetInterface *GetInterface(uint32 ip);
void Poll(NetInterface *intf);
int Send(NetPacket *pkt);
} // namespace PacketManager
