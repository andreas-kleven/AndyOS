#pragma once
#include <Net/netinterface.h>
#include <Net/packet.h>
#include <Net/socket.h>

namespace PacketManager {
int Send(NetPacket *pkt);
void SetInterface(NetInterface *intf);
NetInterface *GetInterface(uint32 ip);
void Poll();
} // namespace PacketManager
