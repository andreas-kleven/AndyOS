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
    std::vector<RawPacket> packet_buffer;

    void AddPacket(uint8_t destination, uint8_t player, uint8_t type, uint8_t length, const void *data);
    void ProcessPacket(NetPacket *packet);
};
