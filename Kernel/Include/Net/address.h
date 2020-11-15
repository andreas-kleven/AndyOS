#pragma once
#include <string.h>
#include <types.h>

#define AF_UNSPEC  0
#define AF_LOCAL   1
#define AF_UNIX    AF_LOCAL
#define AF_INET    2
#define AF_PACKET  3
#define AF_INET6   10
#define AF_NETLINK 16

#define SOCK_STREAM    1
#define SOCK_DGRAM     2
#define SOCK_RAW       3
#define SOCK_RDM       4
#define SOCK_SEQPACKET 5

#define IPPROTO_IP   0
#define IPPROTO_ICMP 1
#define IPPROTO_TCP  6
#define IPPROTO_UDP  17

#define SHUT_RD   0
#define SHUT_WR   1
#define SHUT_RDWR 2

#define INADDR_ANY       (uint32)0x00000000
#define INADDR_BROADCAST (uint32)0xffffffff

typedef uint32 socklen_t;

struct in_addr
{
    unsigned long s_addr;
} __attribute__((packed));

struct sockaddr
{
    unsigned short sa_family;
    char sa_data[14];
} __attribute__((packed));

struct sockaddr_in
{
    unsigned short sin_family;
    unsigned short sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
} __attribute__((packed));

struct sockaddr_un
{
    unsigned short sun_family;
    char sun_path[108];
} __attribute__((packed));

struct iovec
{                   /* Scatter/gather array items */
    void *iov_base; /* Starting address */
    size_t iov_len; /* Number of bytes to transfer */
} __attribute__((packed));

struct msghdr
{
    void *msg_name;        /* optional address */
    socklen_t msg_namelen; /* size of address */
    struct iovec *msg_iov; /* scatter/gather array */
    size_t msg_iovlen;     /* # elements in msg_iov */
    void *msg_control;     /* ancillary data, see below */
    size_t msg_controllen; /* ancillary data buffer len */
    int msg_flags;         /* flags on received message */
} __attribute__((packed));

struct MacAddress
{
    uint8 n[6];

    MacAddress();
    MacAddress(uint8 n0, uint8 n1, uint8 n2, uint8 n3, uint8 n4, uint8 n5);

    bool operator==(const MacAddress &a) const { return !memcmp(n, a.n, 6); }

    bool operator!=(const MacAddress &a) const { return memcmp(n, a.n, 6); }
} __attribute__((packed));
