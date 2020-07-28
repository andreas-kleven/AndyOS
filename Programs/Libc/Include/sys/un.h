#pragma once

struct sockaddr_un
{
    unsigned short sun_family;
    char sun_path[108];
} __attribute__((packed));
