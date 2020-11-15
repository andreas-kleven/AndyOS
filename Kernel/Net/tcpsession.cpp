#include <Net/packetmanager.h>
#include <Net/socketmanager.h>
#include <Net/tcp.h>
#include <debug.h>
#include <errno.h>
#include <math.h>

TcpSession::TcpSession()
{
    ack_event = Event(false, true);
    fin_event = Event(false, true);
    Reset();
}

int TcpSession::Connect(const sockaddr_in *addr)
{
    mutex.Aquire();
    kprintf("Connecting...\n");

    if (state != TCP_CLOSED) {
        mutex.Release();
        return -1;
    }

    socket->addr = (sockaddr *)(new sockaddr_in(*addr));

    seq = rand();
    first_seq = seq;
    state = TCP_SYN_SENT;

    if (!SendWait(TCP_SYN)) {
        mutex.Release();
        return -ETIMEDOUT;
    }

    if (state != TCP_ESTABLISHED) {
        Reset();
        mutex.Release();
        return -ECONNREFUSED;
    }

    kprintf("Connected\n");
    mutex.Release();
    return 0;
}

int TcpSession::Shutdown(int how)
{
    mutex.Aquire();

    kprintf("Closing...\n");

    if (state == TCP_CLOSED)
        return -ENOTCONN;
    // else if (state == ...)

    if (state == TCP_CLOSE_WAIT) {
        state = TCP_LAST_ACK;
        SendWait(TCP_FIN | TCP_ACK);
        kprintf("Closed\n");
        Reset();

        recv_buffer.Shutdown();
        socket->HandleShutdown();
    } else {
        state = TCP_FIN_WAIT_1;

        if (!SendWait(TCP_FIN | TCP_ACK)) {
            mutex.Release();
            return 0;
        }

        if (state == TCP_FIN_WAIT_2)
            fin_event.Wait(TCP_TIMEOUT);

        if (state == TCP_TIME_WAIT) {
            // TODO: delay
            kprintf("Wait closed\n");
            Reset();

            recv_buffer.Shutdown();
            socket->HandleShutdown();
            mutex.Release();
        } else {
            kprintf("Closed with state: %d\n", state);
            Reset();

            recv_buffer.Shutdown();
            socket->HandleShutdown();
            mutex.Release();
        }
    }

    return 0;
}

Socket *TcpSession::Accept(const sockaddr_in *addr, int flags)
{
    while (state == TCP_LISTEN) {
        if (!session_event.WaitIntr())
            return (Socket *)-EINTR;

        sessions_mutex.Aquire();

        if (new_sockets.Count() > 0) {
            Socket *new_socket = new_sockets.Dequeue();

            if (new_sockets.Count() == 0)
                session_event.Clear();

            sessions_mutex.Release();

            if (new_socket->tcp_session->SendWait(TCP_SYN | TCP_ACK))
                return new_socket;
        } else {
            sessions_mutex.Release();
        }
    }

    return 0;
}

int TcpSession::Listen(int backlog)
{
    mutex.Aquire();

    if (state != TCP_CLOSED) {
        mutex.Release();
        return -1;
    }

    this->backlog = backlog;
    state = TCP_LISTEN;
    socket->listening = true;
    mutex.Release();
    return 0;
}

int TcpSession::SendData(const void *buf, size_t len, int flags)
{
    mutex.Aquire();

    if (state != TCP_ESTABLISHED) {
        mutex.Release();
        return -ENOTCONN;
    }

    if (SendWait(TCP_PSH | TCP_ACK, buf, len)) {
        seq += len;
    }

    mutex.Release();
    return 0;
}

int TcpSession::RecvData(void *buf, size_t len, int flags)
{
    return recv_buffer.Recv(buf, len, flags);
}

bool TcpSession::HandlePacket(IPv4_Header *ip_hdr, TCP_Packet *tcp)
{
    TCP_Header *header = tcp->header;
    uint8 flags = tcp->header->flags;
    bool has_data = tcp->data_length > 0;

    // kprintf("Tcp recv %d %d (seq:%d) (seq:%d ack:%d))\n", tcp->data_length, flags,
    //        tcp->header->seq - first_ack, seq - first_seq, ack - first_ack);

    if ((state == TCP_CLOSED || state == TCP_LISTEN) && flags != TCP_SYN)
        return false;

    lock.Aquire();

    switch (state) {
    case TCP_CLOSED:
        if (flags == TCP_SYN) {
            if (state == TCP_CLOSED) {
                seq = rand();
                first_seq = seq;
                ack = header->seq + 1;
                first_ack = header->seq;
                state = TCP_SYN_RECEIVED;
            }
        }
        break;

    case TCP_LISTEN:
        if (flags == TCP_SYN) {
            seq = rand();
            first_seq = seq;
            ack = header->seq + 1;
            first_ack = header->seq;

            kprintf("New session\n");

            sessions_mutex.Aquire();

            if (new_sockets.Count() >= backlog) {
                sessions_mutex.Release();
                break;
            }

            sockaddr_in *addr = new sockaddr_in();
            addr->sin_family = AF_INET;
            addr->sin_port = htons(tcp->header->src_port);
            addr->sin_family = AF_INET;
            addr->sin_addr.s_addr = ip_hdr->src;

            Socket *new_socket = SocketManager::CreateSocket();
            new_socket->Init(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            new_socket->addr = (sockaddr *)addr;
            new_socket->src_port = tcp->header->dst_port;
            new_socket->tcp_session->HandlePacket(ip_hdr, tcp);
            new_sockets.Enqueue(new_socket);

            sessions_mutex.Release();
            session_event.Set();
        }
        break;

    case TCP_SYN_RECEIVED:
        if (flags & TCP_ACK) {
            seq += 1;

            state = TCP_ESTABLISHED;
            ack_event.Set();
            kprintf("Connected\n");
        }
        break;

    case TCP_SYN_SENT:
        ack = header->seq + 1;
        first_ack = header->seq;
        seq += 1;

        if (flags & TCP_RST) {
            Send(TCP_ACK);
            Reset();
            ack_event.Set();
        }

        if (flags & TCP_SYN && flags & TCP_ACK) {
            Send(TCP_ACK);
            state = TCP_ESTABLISHED;
            ack_event.Set();
        }
        break;

    case TCP_ESTABLISHED:
        if (flags & TCP_FIN) {
            state = TCP_CLOSE_WAIT;
            ack += 1;
            Send(TCP_ACK);
            Reset();

            recv_buffer.Shutdown();
            socket->HandleShutdown();
        } else if (has_data) {
            if (recv_buffer.CanReceive(tcp)) {
                if (recv_buffer.HandleReceive(tcp, socket)) {
                    int next_seq = recv_buffer.NextSeq();

                    if (next_seq) {
                        while (next_seq) {
                            ack = next_seq;
                            Send(TCP_ACK);
                            next_seq = recv_buffer.NextSeq();
                        }

                        break;
                    }
                } else {
                    kprintf("Tcp missing packet 1\n");
                }

                Send(TCP_ACK);
            } else {
                kprintf("Tcp dropped %d %d\n", tcp->header->seq - first_ack, tcp->data_length);
            }
        } else {
            // Received ack
        }

        if (flags & TCP_ACK)
            ack_event.Set();
        break;

    case TCP_FIN_WAIT_1:
        seq += 1;

        if (flags == TCP_FIN) {
            state = TCP_CLOSING;
        } else if (flags & TCP_ACK) {
            if (flags & TCP_FIN) {
                state = TCP_TIME_WAIT;
                ack += 1;
            } else
                state = TCP_FIN_WAIT_2;

            Send(TCP_ACK);
            ack_event.Set();
        }
        break;

    case TCP_FIN_WAIT_2:
        if (flags & TCP_FIN) {
            ack += 1;
            Send(TCP_ACK);
            state = TCP_TIME_WAIT;
            fin_event.Set();
        }
        break;

    case TCP_LAST_ACK:
        if (flags & TCP_ACK) {
            // TODO
            seq += 1;
            state = TCP_TIME_WAIT;
            Send(TCP_ACK); //
            kprintf("Passive close\n");
            Reset();

            recv_buffer.Shutdown();
            socket->HandleShutdown();
        }
        break;

    case TCP_CLOSING:
        if (flags & TCP_ACK) {
            state = TCP_TIME_WAIT;
            ack_event.Set();
        }
        break;
    }

    lock.Release();
    return true;
}

bool TcpSession::SendWait(uint8 flags, int timeout)
{
    return SendWait(flags, 0, 0, timeout);
}

bool TcpSession::SendWait(uint8 flags, const void *buf, size_t len, int timeout)
{
    ack_event.Clear();

    if (!Send(flags, ack, buf, len))
        return false;

    if (ack_event.Wait(timeout))
        return true;

    kprintf("Timed out\n");
    return false;
}

bool TcpSession::Send(uint8 flags)
{
    return Send(flags, ack, 0, 0);
}

bool TcpSession::Send(uint8 flags, uint32 _ack, const void *buf, size_t len)
{
    sockaddr_in *addr = (sockaddr_in *)socket->addr;
    NetInterface *intf = PacketManager::GetInterface(addr->sin_addr.s_addr);
    NetPacket *pkt = TCP::CreatePacket(intf, addr, socket->src_port, flags, seq, _ack, buf, len);

    if (!pkt)
        return false;

    TCP::Send(pkt);
    return true;
}

void TcpSession::Reset()
{
    seq = 0;
    ack = 0;
    first_seq = 0;
    first_ack = 0;
    state = TCP_CLOSED;

    ack_event.Set();
    fin_event.Set();
    session_event.Set();

    ack_event.Clear();
    fin_event.Clear();
    session_event.Clear();
}
