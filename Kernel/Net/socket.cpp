#include <Net/packetmanager.h>
#include <Net/socket.h>
#include <Net/socketmanager.h>
#include <Net/tcp.h>
#include <Net/udp.h>
#include <Net/unix.h>
#include <debug.h>
#include <errno.h>
#include <string.h>

Socket::Socket()
{}

Socket::Socket(int id)
{
    this->id = id;
}

int Socket::Init(int domain, int type, int protocol)
{
    this->buffer = new CircularDataBuffer(SOCKET_BUFFER_SIZE);
    this->domain = domain;
    this->type = type;
    this->addr = 0;

    if (protocol == 0) {
        if (domain == AF_INET) {
            if (type == SOCK_STREAM)
                this->protocol = IP_PROTOCOL_TCP;
            else if (type == SOCK_DGRAM)
                this->protocol = IP_PROTOCOL_UDP;
        }
    } else {
        this->protocol = protocol;
    }

    if (this->protocol == IP_PROTOCOL_UDP || this->protocol == IP_PROTOCOL_TCP)
        this->src_port = SocketManager::AllocPort();

    if (this->protocol == IP_PROTOCOL_TCP)
        TCP::CreateSession(this);

    return 0;
}

Socket::~Socket()
{
    delete[] this->addr;
    delete this->buffer;
}

int Socket::Accept(sockaddr *addr, socklen_t addrlen, int flags)
{
    if (!listening)
        return -EINVAL;

    if (domain == AF_UNIX) {
        Socket *socket = Unix::Accept(this);

        if (PTR_ERR(socket))
            return (int)socket;

        if (socket)
            return socket->id;
    } else if (protocol == IPPROTO_TCP) {
        sockaddr_in *inet_addr = (sockaddr_in *)addr;
        Socket *socket = TCP::SessionAccept(this, inet_addr, flags);

        if (PTR_ERR(socket))
            return (int)socket;

        if (socket)
            return socket->id;
    } else {
        return -EINVAL;
    }

    return -1;
}

int Socket::Bind(const sockaddr *addr, socklen_t addrlen)
{
    if (this->addr)
        return -EINVAL;

    if (addrlen < sizeof(sockaddr) || addrlen > SOCKET_ADDR_SIZE || addr->sa_family != this->domain)
        return -EINVAL;

    this->addr = (sockaddr *)(new char[addrlen]);
    memcpy(this->addr, addr, addrlen);

    // TODO
    if (this->addr->sa_family == AF_INET) {
        sockaddr_in *inet_addr = (sockaddr_in *)this->addr;

        if (inet_addr->sin_port == 0)
            inet_addr->sin_port = htons(this->src_port);
    }

    return 0;
}

int Socket::Connect(const sockaddr *addr, socklen_t addrlen)
{
    if (addr->sa_family == AF_UNIX) {
        sockaddr_un *unix_addr = (sockaddr_un *)addr;
        Socket *server = SocketManager::GetUnixSocket(unix_addr);

        if (!server)
            return -ECONNREFUSED;

        if (server->type != type)
            return -ECONNREFUSED;

        return Unix::Connect(this, server);
    } else if (protocol == IPPROTO_TCP) {
        sockaddr_in *inet_addr = (sockaddr_in *)addr;
        return TCP::SessionConnect(this, inet_addr);
    } else {
        return -EINVAL;
    }

    return 0;
}

int Socket::Listen(int backlog)
{
    if (!addr)
        return -1;

    if (domain == AF_UNIX) {
        return Unix::Listen(this);
    } else if (protocol == IPPROTO_TCP) {
        return TCP::SessionListen(this, backlog);
    } else {
        return -1;
    }
}

int Socket::Recv(void *buf, size_t len, int flags)
{
    if (protocol == IPPROTO_TCP)
        return TCP::SessionRecv(this, buf, len, flags);

    if (!recv_event.WaitIntr())
        return -EINTR;

    buffer_mutex.Aquire();

    if (buffer->IsEmpty() && closed) {
        buffer_mutex.Release();
        return -ENOTCONN;
    }

    int ret = buffer->Read(len, buf);

    if (buffer->IsEmpty())
        recv_event.Clear();

    buffer_mutex.Release();
    return ret;
}

int Socket::Recvfrom(void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t addrlen)
{
    // TODO
    int ret = Recv(buf, len, flags);

    if (ret > 0 && src_addr)
        memcpy(src_addr, recv_addr, sizeof(sockaddr_in));

    return ret;
}

int Socket::Recvmsg(msghdr *msg, int flags)
{
    return 0;
}

int Socket::Send(const void *buf, size_t len, int flags)
{
    if (domain == AF_UNIX) {
        return Unix::Send(this, buf, len, flags);
    } else if (protocol == IPPROTO_TCP) {
        return TCP::SessionSend(this, buf, len, flags);
    }

    return -1;
}

int Socket::Sendmsg(const msghdr *msg, int flags)
{
    return 0;
}

int Socket::Sendto(const void *buf, size_t len, int flags, const sockaddr *dest_addr,
                   socklen_t addrlen)
{
    if (protocol == IPPROTO_UDP) {
        sockaddr_in *inet_addr = (sockaddr_in *)dest_addr;
        NetInterface *intf = PacketManager::GetInterface(inet_addr->sin_addr.s_addr);
        NetPacket *pkt = UDP::CreatePacket(intf, inet_addr, src_port, buf, len);
        return UDP::Send(pkt);
    } else {
        return -1;
    }

    return 0;
}

int Socket::Shutdown(int how)
{
    if (protocol == IPPROTO_TCP)
        return TCP::SessionShutdown(this, how);

    return 0;
}

int Socket::Close()
{
    if (protocol == IPPROTO_TCP)
        return TCP::SessionClose(this);

    return 0;
}

void Socket::HandleData(const void *data, int length)
{
    if (!length)
        return;

    buffer_mutex.Aquire();
    buffer->Write(data, length);

    if (!buffer->IsEmpty())
        recv_event.Set();

    buffer_mutex.Release();
}

void Socket::HandleShutdown()
{
    buffer_mutex.Aquire();
    closed = true;
    recv_event.Set();
    buffer_mutex.Release();
}
