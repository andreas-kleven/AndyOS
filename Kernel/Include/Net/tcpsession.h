#pragma once
#include <Net/address.h>
#include <Net/ipv4.h>
#include <Net/socket.h>
#include <Net/tcptypes.h>
#include <queue.h>
#include <sync.h>

#define TCP_TIMEOUT     3000
#define TCP_WINDOW_SIZE 4096

struct TcpLinkPacket
{
    TCP_Packet *pkt;
    TcpLinkPacket *next;
    TcpLinkPacket *prev;
} __attribute__((packed));

class TcpSession
{
  public:
    Socket *socket;
    // sockaddr_in addr;
    int state;
    int backlog;
    Mutex mutex;
    Mutex sessions_mutex;
    Event ack_event;
    Event fin_event;
    Event session_event;
    Queue<Socket *> new_sockets;

    TcpSession();

    int Connect(const sockaddr_in *addr);
    int Close(int how);
    Socket *Accept(const sockaddr_in *addr, int flags);
    int Listen(int backlog);
    int SendData(const void *buf, size_t len, int flags);

    bool HandlePacket(IPv4_Header *ip_hdr, TCP_Packet *tcp);

  private:
    uint32 seq;
    uint32 ack;

    bool SendWait(uint8 flags, int timeout = TCP_TIMEOUT);
    bool SendWait(uint8 flags, const void *buf, size_t len, int timeout = TCP_TIMEOUT);
    bool Send(uint8 flags);
    bool Send(uint8 flags, const void *buf, size_t len);
    void Reset();
    void CloseSocket();
};
