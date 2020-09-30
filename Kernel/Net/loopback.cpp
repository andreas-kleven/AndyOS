#include <Kernel/dpc.h>
#include <Net/eth.h>
#include <Net/loopback.h>
#include <Net/packetmanager.h>

LoopbackInterface::LoopbackInterface() : NetInterface(0)
{}

void LoopbackInterface::Send(NetPacket *pkt)
{
    if (!pkt)
        return;

    packets_mutex.Aquire();
    packets.Enqueue(pkt);
    packets_mutex.Release();

    Dpc::Create((DPC_HANDLER)PacketManager::Poll, this);
}

void LoopbackInterface::Poll()
{
    while (true) {
        NetPacket *pkt = 0;

        packets_mutex.Aquire();

        if (packets.Count() > 0)
            pkt = packets.Dequeue();

        packets_mutex.Release();

        if (!pkt)
            break;

        ETH::HandlePacket(this, pkt);
    }
}
