#pragma once
#include <Net/netinterface.h>
#include <types.h>

#define NET_PACKET_SIZE 0x1000

class NetInterface;

struct NetPacket
{
    NetInterface *interface;
    uint8 *start;
    uint8 *end;
    uint32 length;
};
