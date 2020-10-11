#include "Net/NetManager.h"
#include "Player.h"
#include <Engine.h>
#include <Player.h>
#include <stdio.h>
#include <string.h>

NetworkManager::NetworkManager()
{
    this->socket = new NetSocket();
    this->bServer = false;
    this->bConnected = false;
    this->next_playerid = 2;
    this->next_objectnetid = 2;
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

    ConnectPacket pkt;
    strcpy(pkt.name, "Player name");
    AddPacket(0, 0, 0, PACKETTYPE_CONNECT, &pkt, sizeof(pkt));

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

void NetworkManager::AddPacket(uint8_t destination, uint8_t player, uint32_t object, uint8_t type,
                               const void *data, uint8_t length)
{
    RawPacket pkt;
    pkt.destination = destination;
    pkt.player = player;
    pkt.object = object;
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
    printf("Packet %d\n", packet->type);

    switch (packet->type) {
    case PACKETTYPE_CONNECT: {
        if (!IsServer())
            return;

        ConnectPacket *pkt = (ConnectPacket *)packet->data;
        printf("Player connect: '%s'\n", pkt->name);
        Player *player = PlayerManager::CreatePlayer(next_playerid++, pkt->name);
        GameObject *player_object = GEngine::game->CreatePlayer();

        if (player_object) {
            player_object->SetNetId(next_objectnetid++);
            player_object->SetOwner(player);
            player_object->Start();
        }

        // response
        ConnectedPacket conpkt;
        conpkt.self = true;
        strcpy(conpkt.name, player->name.c_str());
        AddPacket(packet->source, player->id, 0, PACKETTYPE_CONNECTED, &conpkt, sizeof(conpkt));

        // send all players
        PlayerManager::SetPlayer(0);

        do {
            Player *p = PlayerManager::GetCurrentPlayer();
            conpkt.self = false;

            if (p->id != player->id) {
                strcpy(conpkt.name, p->name.c_str());
                AddPacket(packet->source, p->id, 0, PACKETTYPE_CONNECTED, &conpkt, sizeof(conpkt));
            }
        } while (PlayerManager::NextPlayer());

        // send spawn
        for (auto &object : GEngine::game->objects) {
            if (object->GetNetId()) {
                SpawnPacket spawnpkt;
                spawnpkt.transform = object->transform;
                AddPacket(PACKET_BROADCAST, object->GetOwner()->id, object->GetNetId(),
                          PACKETTYPE_SPAWN, &spawnpkt, sizeof(spawnpkt));
            }
        }
    } break;

    case PACKETTYPE_CONNECTED: {
        ConnectedPacket *pkt = (ConnectedPacket *)packet->data;

        if (pkt->self) {
            printf("Self connected %p '%s'\n", packet->player, pkt->name);
            PlayerManager::GetPlayer(0)->id = packet->player;
        } else {
            printf("Player connected %d '%s'\n", packet->player, pkt->name);
            PlayerManager::CreatePlayer(packet->player, pkt->name);
        }
    } break;

    case PACKETTYPE_SPAWN: {
        SpawnPacket *pkt = (SpawnPacket *)packet->data;
        printf("Spawn player %d %d\n", packet->player, packet->object);

        if (PlayerManager::SetPlayer(packet->player)) {
            GameObject *object = GEngine::game->CreatePlayer(pkt->transform);
            object->SetNetId(packet->object);
            object->SetOwner(PlayerManager::GetPlayer(packet->player));
            object->Start();
        }
    } break;
    }
}
