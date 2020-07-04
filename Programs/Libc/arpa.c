#include <arpa/inet.h>

uint16_t htons(uint16_t val)
{
    return (val >> 8) | (val << 8);
}

uint16_t ntohs(uint16_t val)
{
    return (val >> 8) | (val << 8);
}

uint32_t htonl(uint32_t val)
{
    return ((val >> 24) & 0xff) |
           ((val << 8) & 0xff0000) |
           ((val >> 8) & 0xff00) |
           ((val << 24) & 0xff000000);
}

uint32_t ntohl(uint32_t val)
{
    return ((val >> 24) & 0xff) |
           ((val << 8) & 0xff0000) |
           ((val >> 8) & 0xff00) |
           ((val << 24) & 0xff000000);
}
