#pragma once

struct in_addr
{
    unsigned long s_addr;
} __attribute__((packed));

struct sockaddr_in
{
    unsigned short sin_family;
    unsigned short sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
} __attribute__((packed));
