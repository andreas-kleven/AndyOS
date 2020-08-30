#pragma once

#define POLLIN   0x001 /* There is data to read.  */
#define POLLPRI  0x002 /* There is urgent data to read.  */
#define POLLOUT  0x004 /* Writing now will not block.  */
#define POLLERR  0x008 /* Error condition.  */
#define POLLHUP  0x010 /* Hung up.  */
#define POLLNVAL 0x020 /* Invalid polling request.  */

typedef unsigned long int nfds_t;

struct pollfd
{
    int fd;        /* file descriptor */
    short events;  /* requested events */
    short revents; /* returned events */
};
