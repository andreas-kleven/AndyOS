#pragma once
#include <FS/filesystem.h>
#include <list.h>
#include <driver.h>
#include <Net/socket.h>

class SockFS : public FileSystem
{
public:
    SockFS()
    {
        name = "sockfs";
    }

    int Mount(BlockDriver *driver);
    int Open(FILE *file);
    int Close(FILE *file);
    int Read(FILE *file, void *buf, size_t size);
    int Write(FILE *file, const void *buf, size_t size);
    int Seek(FILE *file, long offset, int origin);
    int GetChildren(DENTRY *parent, const char *find_name);

    int Create(int domain, int type, int protocol, DENTRY *&dentry);
    int Accept(FILE *file, sockaddr *addr, socklen_t addrlen, int flags, DENTRY *&dentry);
    int Bind(FILE *file, const sockaddr *addr, socklen_t addrlen);
    int Connect(FILE *file, const sockaddr *addr, socklen_t addrlen);
    int Listen(FILE *file, int backlog);
    int Recv(FILE *file, void *buf, size_t len, int flags);
    int Recvfrom(FILE *file, void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t addrlen);
    int Send(FILE *file, const void *buf, size_t len, int flags);
    int Sendto(FILE *file, const void *buf, size_t len, int flags, const sockaddr *dest_addr, socklen_t addrlen);
    int Shutdown(FILE *file, int how);

private:
    Socket *GetSocket(FILE *file);
    DENTRY *CreateSocketDentry(int socket_id);
};
