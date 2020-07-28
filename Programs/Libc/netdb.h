#pragma once

#define AI_PASSIVE 0x00000001     /* get address to use bind() */
#define AI_CANONNAME 0x00000002   /* fill ai_canonname */
#define AI_NUMERICHOST 0x00000004 /* prevent name resolution */
#define AI_NUMERICSERV 0x00000008 /* don't use name resolution. */

#define NI_NOFQDN 0x00000001
#define NI_NUMERICHOST 0x00000002
#define NI_NAMEREQD 0x00000004
#define NI_NUMERICSERV 0x00000008
#define NI_DGRAM 0x00000010
#define NI_WITHSCOPEID 0x00000020

struct hostent
{
    char *h_name;
    char **h_aliases;
    int h_addrtype;
    int h_length;
    char **h_addr_list;
};

#define h_addr h_addr_list[0]

struct addrinfo
{
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    socklen_t ai_addrlen;
    struct sockaddr *ai_addr;
    char *ai_canonname;
    struct addrinfo *ai_next;
};

struct hostent *gethostbyname(const char *name);
