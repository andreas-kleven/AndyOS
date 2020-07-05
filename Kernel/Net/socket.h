#pragma once
#include "types.h"
#include "address.h"
#include "circbuf.h"

#define SOCKET_BUFFER_SIZE 16384
#define SOCKET_ADDR_SIZE 256

class Socket
{
public:
    int id;
    int domain;
    int type;
    int protocol;
    int options;
    int src_port;
    sockaddr *addr;
    CircularDataBuffer buffer;
    Mutex buffer_mutex;
    Event read_event;
    Event accept_event;
    Event connect_event;
    Mutex connect_mutex;
    Socket *tmp_client;
    Socket *unix_socket;
    bool listening;

    Socket();
    Socket(int id, int domain, int type, int protocol);
    ~Socket();

    int Accept(sockaddr *addr, socklen_t addrlen, int flags);
    int Bind(const sockaddr *addr, socklen_t addrlen);
    int Connect(const sockaddr *addr, socklen_t addrlen);
    int Listen(int backlog);
    int Recv(void *buf, size_t len, int flags);
    int Recvfrom(void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t addrlen);
    int Recvmsg(msghdr *msg, int flags);
    int Send(const void *buf, size_t len, int flags);
    int Sendmsg(const msghdr *msg, int flags);
    int Sendto(const void *buf, size_t len, int flags, const sockaddr *dest_addr, socklen_t addrlen);
    int Shutdown(int how);

    void HandleData(const void *data, int length);
};
