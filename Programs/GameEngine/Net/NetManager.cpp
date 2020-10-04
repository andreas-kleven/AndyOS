#include "Net/NetManager.h"
#include "Player.h"
#include <stdio.h>
#include <string.h>

NetworkManager::NetworkManager()
{
    this->socket = new NetSocket();
    this->bServer = false;
    this->bConnected = false;
}

bool NetworkManager::Host(int port)
{
    if (!socket->Host(port))
        return false;

    bServer = true;
    bConnected = true;
    return true;
}

bool NetworkManager::Connect(int port)
{
    if (!socket->Connect(port))
        return false;

    const char *playername = "Player name";
    AddPacket(0, 0, PACKETTYPE_CONNECT, strlen(playername), playername);

    bConnected = true;

    return true;
}

bool NetworkManager::Disconnect()
{
    return socket->Disconnect();
}

void NetworkManager::SendPackets()
{
    socket->Send(packet_buffer);
    packet_buffer.clear();
}

void NetworkManager::AddPacket(uint8_t destination, uint8_t player, uint8_t type, uint8_t length,
                               const void *data)
{
    RawPacket pkt;
    pkt.destination = destination;
    pkt.player = player;
    pkt.type = type;
    pkt.length = length;
    memcpy(pkt.data, data, length);
    packet_buffer.push_back(pkt);
}

void NetworkManager::ProcessPackets()
{
    NetPacket *pkt;

    while ((pkt = socket->GetNextPacket())) {
        ProcessPacket(pkt);
    }
}

void NetworkManager::ProcessPacket(NetPacket *packet)
{
    switch (packet->type) {
    case PACKETTYPE_CONNECT:
        const char *playername = (char *)packet->data;
        printf("Player connected: '%s'\n", playername);
        PlayerManager::CreatePlayer(packet->player, playername);

        if (IsServer()) {
            Player *player = PlayerManager::GetPlayer(0);
            const char *name = player->name.c_str();
            AddPacket(PACKET_BROADCAST, packet->player, PACKETTYPE_CONNECT, strlen(name), name);
        }
        break;
    }
}
