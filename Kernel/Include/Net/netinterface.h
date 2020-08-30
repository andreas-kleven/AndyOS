#pragma once
#include <Net/address.h>
#include <Net/packet.h>
#include <pci.h>
#include <types.h>

class NetInterface /*: public PciDevice*/
{
  protected:
    PciDevice *pci_dev;

  public:
    uint32 gateway_addr;

    NetInterface(PciDevice *pci_dev);

    virtual void Send(NetPacket *pkt) {}
    virtual void Poll() {}

    // static void(*EthReceive)(NetInterface* intf, NetPacket* pkt);

    virtual MacAddress GetMac() { return MacAddress(); }
    virtual uint32 GetIP() { return 0; }
};
