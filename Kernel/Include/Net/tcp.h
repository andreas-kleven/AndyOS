#pragma once
#include <Net/ipv4.h>
#include <Net/tcpsession.h>
#include <Net/tcptypes.h>

namespace TCP {
int CreateSession(Socket *socket);
Socket *SessionAccept(Socket *socket, const sockaddr_in *addr, int flags);
int SessionShutdown(Socket *socket, int how);
int SessionClose(Socket *socket);
int SessionListen(Socket *socket, int backlog);
int SessionConnect(Socket *socket, const sockaddr_in *addr);
int SessionSend(Socket *socket, const void *buf, size_t len, int flags);
int SessionRecv(Socket *socket, void *buf, size_t len, int flags);

NetPacket *CreatePacket(NetInterface *intf, const sockaddr_in *dest_addr, uint16 src_port,
                        uint8 flags, uint32 seq, uint32 ack, const void *data, uint32 data_length);
void Send(NetPacket *pkt);
void HandlePacket(IPv4_Header *ip_hdr, NetPacket *pkt);
}; // namespace TCP
