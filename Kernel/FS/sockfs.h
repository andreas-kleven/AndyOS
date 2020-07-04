#pragma once
#include "filesystem.h"
#include "list.h"
#include "Drivers/driver.h"
#include "Net/socket.h"

class SockFS : public FileSystem
{
public:
    SockFS()
    {
        name = "sockfs";
    }

    int Mount(BlockDriver *driver, DENTRY *root_dentry);
    int Open(FILE *file);
    int Close(FILE *file);
    int Read(FILE *file, void *buf, size_t size);
    int Write(FILE *file, const void *buf, size_t size);
    int Seek(FILE *file, long offset, int origin);
    int GetChildren(DENTRY *parent, const char *find_name);

    int Create(DENTRY *&dentry, int domain, int type, int protocol);
    int Bind(FILE *file, const struct sockaddr *addr, socklen_t addrlen);
    int Recv(FILE *file, void *buf, size_t len, int flags);
    int Sendto(FILE *file, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
    int Shutdown(FILE *file, int how);

private:
    Socket *GetSocket(FILE *file);
};
