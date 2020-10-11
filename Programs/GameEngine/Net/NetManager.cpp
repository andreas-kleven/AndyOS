#include "Net/NetManager.h"
#include "Player.h"
#include <Engine.h>
#include <Player.h>
#include <keycodes.h>
#include <stdio.h>
#include <string.h>

struct ConnectPacket
{
    char name[32];
};

struct ConnectedPacket
{
    bool self;
    char name[32];
};

struct SpawnPacket
{
    Transform transform;
};

struct KeyInputPacket
{
    KEYCODE key;
    bool state;
};

struct AxisInputPacket
{
    INPUT_AXIS axis;
    float value;
};

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

bool NetworkManager::SendKey(KEYCODE key, bool state, uint8_t destination)
{
    KeyInputPacket pkt;
    pkt.key = key;
    pkt.state = state;

    AddPacket(destination, PlayerManager::GetCurrentPlayer()->id, 0, PACKETTYPE_KEYINPUT, &pkt,
              sizeof(pkt));
    return true;
}

bool NetworkManager::SendAxis(INPUT_AXIS axis, float value, uint8_t destination)
{
    AxisInputPacket pkt;
    pkt.axis = axis;
    pkt.value = value;

    AddPacket(destination, PlayerManager::GetCurrentPlayer()->id, 0, PACKETTYPE_AXISINPUT, &pkt,
              sizeof(pkt));
    return true;
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

        PlayerManager::SetPlayer(0);

        // send all players
        do {
            Player *p = PlayerManager::GetCurrentPlayer();
            uint8_t destination = p->id == player->id ? ~packet->source : packet->source;
            conpkt.self = false;
            strcpy(conpkt.name, p->name.c_str());
            AddPacket(destination, p->id, 0, PACKETTYPE_CONNECTED, &conpkt, sizeof(conpkt));
        } while (PlayerManager::NextPlayer());

        // send spawn
        for (auto &object : GEngine::game->objects) {
            if (object->GetNetId()) {
                uint8_t destination = object == player_object ? PACKET_BROADCAST : packet->source;
                SpawnPacket spawnpkt;
                spawnpkt.transform = object->transform;
                AddPacket(destination, object->GetOwner()->id, object->GetNetId(), PACKETTYPE_SPAWN,
                          &spawnpkt, sizeof(spawnpkt));
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

    case PACKETTYPE_KEYINPUT: {
        KeyInputPacket *pkt = (KeyInputPacket *)packet->data;
        //printf("Key input %d %d %d\n", packet->player, pkt->key, pkt->state);

        if (PlayerManager::SetPlayer(packet->player)) {
            if (Input::SetKey(pkt->key, pkt->state)) {
                if (IsServer())
                    SendKey(pkt->key, pkt->state, ~packet->source);
            }
        }

    } break;

    case PACKETTYPE_AXISINPUT: {
        AxisInputPacket *pkt = (AxisInputPacket *)packet->data;
        //printf("Axis input %d %d %.1f\n", packet->player, pkt->axis, pkt->value);

        if (PlayerManager::SetPlayer(packet->player)) {
            if (Input::SetAxis(pkt->axis, pkt->value)) {
                if (IsServer())
                    SendAxis(pkt->axis, pkt->value, ~packet->source);
            }
        }

    } break;
    }
}
