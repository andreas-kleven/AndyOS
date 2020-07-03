#pragma once
#include "types.h"

#define NET_PACKET_SIZE 0x1000

struct NetPacket
{
    uint8 *start;
    uint8 *end;
    uint32 length;
};
