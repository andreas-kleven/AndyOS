#include "Net/NetSocket.h"
#include <AndyOS.h>
#include <andyos/math.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

NetSocket::NetSocket()
{
    this->sockfd = 0;
    this->prev_packet = 0;
}

bool NetSocket::Host(int port)
{
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sockfd < 0) {
        perror("host game socket");
        return false;
    }

    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = INADDR_ANY;
    socket_addr.sin_port = htons(port);

    if (bind(sockfd, (sockaddr *)&socket_addr, sizeof(socket_addr)) < 0) {
        perror("host game bind");
        return false;
    }

    pthread_create(0, 0, ListenLoop, this);
    return true;
}

bool NetSocket::Connect(int port)
{
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sockfd < 0) {
        perror("connect game");
        return false;
    }

    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = htonl(0x7F000001); // 127.0.0.1
    socket_addr.sin_port = 0;

    if (bind(sockfd, (sockaddr *)&socket_addr, sizeof(socket_addr)) < 0) {
        perror("connect game bind");
        return false;
    }

    socket_addr.sin_port = htons(port);

    pthread_create(0, 0, ListenLoop, this);
    return true;
}

bool NetSocket::Disconnect()
{
    return false;
}

bool NetSocket::Send(const std::vector<RawPacket> &packets)
{
    for (auto &rawpkt : packets) {
        size_t size = sizeof(RawPacket) - sizeof(rawpkt.data) + rawpkt.length;

        if (socket_addr.sin_addr.s_addr == INADDR_ANY) {
            // server
            if (rawpkt.destination == PACKET_BROADCAST) {
                // broadcast
                for (auto &entry : addresses) {
                    sockaddr_in addr = entry.addr;
                    sendto(sockfd, &rawpkt, size, 0, (sockaddr *)&addr, sizeof(addr));
                }
            } else if ((int8_t)rawpkt.destination < 0) {
                // broadcast except
                uint8_t except = ~rawpkt.destination;

                for (auto &entry : addresses) {
                    if (entry.player != except) {
                        sockaddr_in addr = entry.addr;
                        sendto(sockfd, &rawpkt, size, 0, (sockaddr *)&addr, sizeof(addr));
                    }
                }
            } else {
                // unicast
                sockaddr_in addr = GetAddress(rawpkt.destination);
                sendto(sockfd, &rawpkt, size, 0, (sockaddr *)&addr, sizeof(addr));
            }
        } else {
            // client
            sendto(sockfd, &rawpkt, size, 0, (sockaddr *)&socket_addr, sizeof(socket_addr));
        }
    }

    return true;
}

NetPacket *NetSocket::GetNextPacket()
{
    if (prev_packet) {
        delete prev_packet;
        prev_packet = 0;
    }

    // TODO: locking
    if (received_packets.size() == 0)
        return 0;

    prev_packet = received_packets.front();
    received_packets.pop_front();
    return prev_packet;
}

void *NetSocket::ListenLoop(void *arg)
{
    NetSocket *inst = (NetSocket *)arg;
    uint8_t recv_buf[sizeof(RawPacket)];
    sockaddr_in recv_addr;

    printf("Listening...\n");

    while (true) {
        int recv_len = recvfrom(inst->sockfd, recv_buf, sizeof(recv_buf), 0, (sockaddr *)&recv_addr,
                                sizeof(recv_addr));

        // printf("Recv %lu %d %d\n", inst->socket_addr.sin_addr.s_addr, recv_len,
        // recv_addr.sin_port);

        if (inst->GetPlayer(recv_addr) == 0) {
            PlayerAddress entry;
            entry.player = inst->addresses.size() + 1;
            entry.addr = recv_addr;
            inst->addresses.push_back(entry);
        }

        uint8_t *ptr = recv_buf;

        while (recv_len > 0) {
            RawPacket *rawpkt = (RawPacket *)ptr;
            NetPacket *pkt = new NetPacket();
            pkt->source = inst->GetPlayer(recv_addr);
            pkt->player = rawpkt->player;
            pkt->object = rawpkt->object;
            pkt->type = rawpkt->type;
            pkt->length = rawpkt->length;
            memcpy(pkt->data, rawpkt->data, min((size_t)pkt->length, sizeof(pkt->data)));

            // TODO: locking
            inst->received_packets.push_back(pkt);

            ptr += rawpkt->length + sizeof(RawPacket) - sizeof(rawpkt->data);
            recv_len -= rawpkt->length + sizeof(RawPacket) - sizeof(rawpkt->data);
        }
    }
}

sockaddr_in NetSocket::GetAddress(int player)
{
    // TODO: locking
    for (auto &entry : addresses) {
        if (entry.player == player)
            return entry.addr;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    return addr;
}

int NetSocket::GetPlayer(const sockaddr_in &addr)
{
    // TODO: locking
    for (auto &entry : addresses) {
        if (entry.addr.sin_addr.s_addr == addr.sin_addr.s_addr &&
            entry.addr.sin_port == addr.sin_port)
            return entry.player;
    }

    return 0;
}
