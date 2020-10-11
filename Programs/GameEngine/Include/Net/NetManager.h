#pragma once
#include "Net/NetSocket.h"
#include <Input.h>

class NetworkManager
{
  public:
    NetworkManager();

    bool Host(int port);
    bool Connect(int port);
    bool Disconnect();
    bool SendKey(KEYCODE key, bool state, uint8_t destination = PACKET_BROADCAST);
    bool SendAxis(INPUT_AXIS axis, float value, uint8_t destination = PACKET_BROADCAST);

    void SendPackets();
    void ProcessPackets();

    inline bool IsServer() { return bServer; }
    inline bool IsClient() { return bClient; }
    inline bool IsConnected() { return bConnected; }

  private:
    NetSocket *socket;
    bool bServer;
    bool bClient;
    bool bConnected;
    int next_playerid;
    int next_objectnetid;
    std::vector<RawPacket> packet_buffer;

    void AddPacket(uint8_t destination, uint8_t player, uint32_t object, uint8_t type,
                   const void *data, uint8_t length);
    void ProcessPacket(NetPacket *packet);
};
