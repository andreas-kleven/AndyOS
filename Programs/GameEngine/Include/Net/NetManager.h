#pragma once
#include "Net/NetSocket.h"

class NetworkManager
{
  public:
    NetworkManager();

    virtual bool Host(int port);
    virtual bool Connect(int port);
    virtual bool Disconnect();

    void SendPackets();
    void ProcessPackets();

    inline bool IsServer() { return bServer; }
    inline bool IsConnected() { return bConnected; }

  private:
    NetSocket *socket;
    bool bServer;
    bool bConnected;
    int next_playerid;
    int next_objectnetid;
    std::vector<RawPacket> packet_buffer;

    void AddPacket(uint8_t destination, uint8_t player, uint32_t object, uint8_t type,
                   const void *data, uint8_t length);
    void ProcessPacket(NetPacket *packet);
};
