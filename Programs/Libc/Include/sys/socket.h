#pragma once
#include <sys/types.h>

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

#define INADDR_ANY       (uint32_t)0x00000000
#define INADDR_BROADCAST (uint32_t)0xffffffff

typedef uint32_t socklen_t;
typedef unsigned short sa_family_t;

struct sockaddr
{
    sa_family_t sa_family;
    char sa_data[14];
} __attribute__((packed));

struct iovec
{
    void *iov_base;
    size_t iov_len;
} __attribute__((packed));

struct msghdr
{
    void *msg_name;
    socklen_t msg_namelen;
    struct iovec *msg_iov;
    size_t msg_iovlen;
    void *msg_control;
    size_t msg_controllen;
    int msg_flags;
} __attribute__((packed));

int socket(int domain, int type, int protocol);
int accept(int fd, struct sockaddr *addr, socklen_t addrlen, int flags);
int bind(int fd, const struct sockaddr *addr, socklen_t addrlen);
int connect(int fd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int fd, int backlog);
int recv(int fd, void *buf, size_t len, int flags);
int recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *src_addr,
             socklen_t addrlen);
int recvmsg(int fd, struct msghdr *msg, int flags);
int send(int fd, const void *buf, size_t len, int flags);
int sendmsg(int fd, const struct msghdr *msg, int flags);
int sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr,
           socklen_t addrlen);
int shutdown(int fd, int how);

const char *hstrerror(int err);
extern int h_errno;

struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type);
