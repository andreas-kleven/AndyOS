#pragma once
#include <Net/netinterface.h>
#include <queue.h>
#include <sync.h>

#define LOOPBACK_ADDRESS 0x7F000001

class LoopbackInterface : public NetInterface
{
  public:
    LoopbackInterface();

    void Send(NetPacket *pkt);
    void Poll();
    inline uint32 GetIP() { return htonl(LOOPBACK_ADDRESS); }
    inline uint32 GetMask() { return htonl(0xFF000000); }

  private:
    Queue<NetPacket *> packets;
    Mutex packets_mutex;
};
