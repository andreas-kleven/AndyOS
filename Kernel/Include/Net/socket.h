#pragma once
#include <Net/address.h>
#include <circbuf.h>
#include <types.h>

#define SOCKET_BUFFER_SIZE 16384
#define SOCKET_ADDR_SIZE   256

class TcpSession;

class Socket
{
  public:
    int id;
    int domain;
    int type;
    int protocol;
    int options;
    uint16 src_port;
    sockaddr *addr;
    sockaddr *recv_addr;
    CircularDataBuffer *buffer;
    Mutex buffer_mutex;
    Event recv_event;
    Event accept_event;
    Event connect_event;
    Mutex connect_mutex;
    Socket *tmp_client;
    Socket *unix_socket;
    TcpSession *tcp_session;
    bool listening = false;
    bool closed = false;

    Socket();
    Socket(int id);
    ~Socket();

    int Init(int domain, int type, int protocol);
    int Accept(sockaddr *addr, socklen_t addrlen, int flags);
    int Bind(const sockaddr *addr, socklen_t addrlen);
    int Connect(const sockaddr *addr, socklen_t addrlen);
    int Listen(int backlog);
    int Recv(void *buf, size_t len, int flags);
    int Recvfrom(void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t addrlen);
    int Recvmsg(msghdr *msg, int flags);
    int Send(const void *buf, size_t len, int flags);
    int Sendmsg(const msghdr *msg, int flags);
    int Sendto(const void *buf, size_t len, int flags, const sockaddr *dest_addr,
               socklen_t addrlen);
    int Shutdown(int how);
    int Close();

    void HandleData(const void *data, int length);
    void HandleShutdown();
};
