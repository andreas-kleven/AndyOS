#pragma once
#include "socket.h"

namespace SocketManager
{
    Socket *CreateSocket(int domain, int type, int protocol);
    int Shutdown(Socket *socket, int how);
    int AllocPort();
    Socket *GetSocket(int id);
    Socket *GetSocket(int domain, int type, int protocol, const sockaddr *addr);
    Socket *GetUdpSocket(unsigned short port);
    Socket *GetUnixSocket(const sockaddr_un *addr);
}; // namespace SocketManager
