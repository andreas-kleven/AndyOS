#include "socket.h"
#include "socketmanager.h"
#include "packetmanager.h"
#include "udp.h"
#include "unix.h"
#include "errno.h"
#include "string.h"
#include "debug.h"

Socket::Socket()
{
}

Socket::Socket(int id, int domain, int type, int protocol)
{
    this->buffer = CircularDataBuffer(SOCKET_BUFFER_SIZE);
    this->id = id;
    this->domain = domain;
    this->type = type;
    this->addr = 0;

    if (protocol == 0)
    {
        if (domain == AF_INET)
        {
            if (type == SOCK_STREAM)
                this->protocol = IP_PROTOCOL_TCP;
            else if (type == SOCK_DGRAM)
                this->protocol = IP_PROTOCOL_UDP;
        }
    }
    else
    {
        this->protocol = protocol;
    }

    if (protocol == IP_PROTOCOL_UDP || protocol == IP_PROTOCOL_TCP)
        this->src_port = SocketManager::AllocPort();
}

Socket::~Socket()
{
    delete this->addr;
}

int Socket::Accept(sockaddr *addr, socklen_t addrlen, int flags)
{
    if (!listening)
        return -EINVAL;

    if (domain == AF_UNIX)
    {
        Socket *socket = Unix::Accept(this);

        if (socket)
            return socket->id;
    }
    else
    {
        return -EINVAL;
    }

    return 0;
}

int Socket::Bind(const sockaddr *addr, socklen_t addrlen)
{
    if (this->addr)
        return -EINVAL;

    if (addrlen < sizeof(sockaddr) || addrlen > SOCKET_ADDR_SIZE || addr->sa_family != this->domain)
        return -EINVAL;

    this->addr = (sockaddr *)(new char[addrlen]);
    memcpy(this->addr, addr, addrlen);
    return 0;
}

int Socket::Connect(const sockaddr *addr, socklen_t addrlen)
{
    if (addr->sa_family == AF_UNIX)
    {
        sockaddr_un *unix_addr = (sockaddr_un *)addr;
        Socket *server = SocketManager::GetUnixSocket(unix_addr);

        if (!server)
            return -ECONNREFUSED;

        if (server->type != type)
            return -ECONNREFUSED;

        return Unix::Connect(this, server);
    }
    else
    {
        return -EINVAL;
    }

    return 0;
}

int Socket::Listen(int backlog)
{
    if (!addr)
        return -1;

    if (domain == AF_UNIX)
    {
        return Unix::Listen(this);
    }
    else
    {
        return -1;
    }
}

int Socket::Recv(void *buf, size_t len, int flags)
{
    read_event.Wait();
    buffer_mutex.Aquire();

    int ret = buffer.Read(len, buf);

    if (buffer.IsEmpty())
        read_event.Clear();

    buffer_mutex.Release();
    return ret;
}

int Socket::Recvfrom(void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t addrlen)
{
    return 0;
}

int Socket::Recvmsg(msghdr *msg, int flags)
{
    return 0;
}

int Socket::Send(const void *buf, size_t len, int flags)
{
    if (domain == AF_UNIX)
    {
        return Unix::Send(this, buf, len, flags);
    }

    return -1;
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

void Socket::HandleData(const void *data, int length)
{
    debug_print("Socket receive %d %d\n", length, PMem::NumFree());

    if (!length)
        return;

    buffer_mutex.Aquire();
    buffer.Write(data, length);

    if (!buffer.IsEmpty())
        read_event.Set();

    buffer_mutex.Release();
}
