#pragma once
#include <Transform.h>
#include <deque>
#include <map>
#include <netinet/in.h>
#include <stdint.h>
#include <vector>

#define PACKET_BROADCAST 255

#define PACKETTYPE_CONNECT   1
#define PACKETTYPE_CONNECTED 2
#define PACKETTYPE_SPAWN     3
#define PACKETTYPE_KEYINPUT  4
#define PACKETTYPE_AXISINPUT 5

struct RawPacket
{
    uint8_t seq;
    uint8_t destination;
    uint8_t player;
    uint32_t object;
    uint8_t type;
    uint8_t length;
    uint8_t data[256];
};

struct NetPacket
{
    uint8_t source;
    uint8_t player;
    uint32_t object;
    uint8_t type;
    uint8_t length;
    uint8_t data[256];
};

struct PlayerAddress
{
    int player;
    sockaddr_in addr;
};

class NetSocket
{
  public:
    NetSocket();

    bool Host(int port);
    bool Connect(int port);
    bool Disconnect();

    bool Send(const std::vector<RawPacket> &packets);
    NetPacket *GetNextPacket();

    sockaddr_in GetAddress(int player);
    int GetPlayer(const sockaddr_in &addr);

  private:
    int sockfd;
    sockaddr_in socket_addr;
    NetPacket *prev_packet;
    std::deque<NetPacket *> received_packets;
    std::vector<PlayerAddress> addresses;

    static void *ListenLoop(void *arg);
};
