#pragma once
#include "types.h"
#include "address.h"
#include "circbuf.h"

#define SOCKET_BUFFER_SIZE 16384

class Socket
{
public:
    int domain;
    int type;
    int protocol;
    int options;
    int src_port;
    sockaddr addr;
    CircularDataBuffer buffer;
    Event read_event;

    Socket();
    Socket(int domain, int type, int protocol);

    int Accept(sockaddr *addr, socklen_t *addrlen, int flags);
    int Bind(const sockaddr *addr, socklen_t addrlen);
    int Connect(const sockaddr *addr, socklen_t addrlen);
    int Listen(int backlog);
    int Recv(void *buf, size_t len, int flags);
    int Recvfrom(void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t *addrlen);
    int Recvmsg(msghdr *msg, int flags);
    int Send(const void *buf, size_t len, int flags);
    int Sendmsg(const msghdr *msg, int flags);
    int Sendto(const void *buf, size_t len, int flags, const sockaddr *dest_addr, socklen_t addrlen);
    int Shutdown(int how);

    void HandleData(void *data, int length);
};
