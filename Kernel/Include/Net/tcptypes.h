#pragma once
#include <net.h>

#define TCP_FIN (1 << 0)
#define TCP_SYN (1 << 1)
#define TCP_RST (1 << 2)
#define TCP_PSH (1 << 3)
#define TCP_ACK (1 << 4)
#define TCP_URG (1 << 5)
#define TCP_ECE (1 << 6)
#define TCP_CWR (1 << 7)

enum TCP_STATE
{
    TCP_LISTEN,
    TCP_SYN_SENT,
    TCP_SYN_RECEIVED,
    TCP_ESTABLISHED,
    TCP_FIN_WAIT_1,
    TCP_FIN_WAIT_2,
    TCP_CLOSE_WAIT,
    TCP_CLOSING,
    TCP_LAST_ACK,
    TCP_TIME_WAIT,
    TCP_CLOSED
};

struct TCP_Header
{
    uint16 src_port;
    uint16 dst_port;
    uint32 seq;
    uint32 ack;
    uint8 offset;
    uint8 flags;
    uint16 window_size;
    uint16 checksum;
    uint16 urg_ptr;
} __attribute__((packed));

struct TCP_Packet
{
    TCP_Header *header;
    uint8 *data;
    uint32 data_length;
} __attribute__((packed));
