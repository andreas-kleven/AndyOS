#include "socket.h"
#include "socketmanager.h"
#include "packetmanager.h"
#include "udp.h"
#include "errno.h"
#include "string.h"
#include "debug.h"

Socket::Socket()
{
}

Socket::Socket(int id, int domain, int type, int protocol)
{
    this->id = id;
    this->domain = domain;
    this->type = type;
    this->protocol = protocol;
    this->buffer = CircularDataBuffer(SOCKET_BUFFER_SIZE);

    if (protocol == IP_PROTOCOL_UDP || protocol == IP_PROTOCOL_TCP)
        this->src_port = SocketManager::AllocPort();
}

int Socket::Accept(sockaddr *addr, socklen_t *addrlen, int flags)
{
    return 0;
}

int Socket::Bind(const sockaddr *addr, socklen_t addrlen)
{
    if (addrlen > sizeof(this->addr))
        return -EINVAL;

    memcpy(&this->addr, addr, addrlen);
    return 0;
}

int Socket::Connect(const sockaddr *addr, socklen_t addrlen)
{
    return 0;
}

int Socket::Listen(int backlog)
{
    return 0;
}

int Socket::Recv(void *buf, size_t len, int flags)
{
    read_event.Wait();
    int ret = buffer.Read(len, buf);

    if (buffer.IsEmpty())
        read_event.Clear();

    return ret;
}

int Socket::Recvfrom(void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t *addrlen)
{
    return 0;
}

int Socket::Recvmsg(msghdr *msg, int flags)
{
    return 0;
}

int Socket::Send(const void *buf, size_t len, int flags)
{
    return 0;
}

int Socket::Sendmsg(const msghdr *msg, int flags)
{
    return 0;
}

int Socket::Sendto(const void *buf, size_t len, int flags, const sockaddr *dest_addr, socklen_t addrlen)
{
    if (protocol == IPPROTO_UDP)
    {
        sockaddr_in *inet_addr = (sockaddr_in *)dest_addr;
        NetInterface *intf = PacketManager::GetInterface(inet_addr);
        NetPacket *pkt = UDP::CreatePacket(inet_addr, src_port, buf, len);
        UDP::Send(pkt);
    }
    else
    {
        return -1;
    }

    return 0;
}

int Socket::Shutdown(int how)
{
    return 0;
}

void Socket::HandleData(void *data, int length)
{
    debug_print("Socket receive %d\n", length);

    if (!length)
        return;

    buffer.Write(data, length);

    if (!buffer.IsEmpty())
        read_event.Set();
}
