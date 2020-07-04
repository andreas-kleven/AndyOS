#include "sockfs.h"
#include "vfs.h"
#include "string.h"
#include "Net/socketmanager.h"
#include "Lib/debug.h"

int SockFS::Mount(BlockDriver *driver, DENTRY *root_dentry)
{
    root_dentry->inode = VFS::AllocInode(root_dentry);
    return -1;
}

int SockFS::Open(FILE *file)
{
    return -1;
}

int SockFS::Close(FILE *file)
{
    Socket *socket = GetSocket(file);

    if (!socket)
        return -1;

    //return socket->Shutdown();
    return -1;
}

int SockFS::Read(FILE *file, void *buf, size_t size)
{
    return Recv(file, buf, size, 0);
}

int SockFS::Write(FILE *file, const void *buf, size_t size)
{
    return -1;
    //return Send(file, buf, size);
}

int SockFS::Seek(FILE *file, long offset, int origin)
{
    return -1;
}

int SockFS::GetChildren(DENTRY *parent, const char *find_name)
{
    return -1;
}

int SockFS::Create(DENTRY *&dentry, int domain, int type, int protocol)
{
    Socket *socket = SocketManager::CreateSocket(domain, type, protocol);

    dentry = VFS::AllocDentry(0, 0);
    INODE *inode = VFS::AllocInode(dentry);
    dentry->type = INODE_TYPE_SOCKET;
    inode->type = INODE_TYPE_SOCKET;
    inode->ino = socket->id;

    return 0;
}

int SockFS::Bind(FILE *file, const struct sockaddr *addr, socklen_t addrlen)
{
    Socket *socket = GetSocket(file);

    if (!socket)
        return -1;

    return socket->Bind(addr, addrlen);
}

int SockFS::Recv(FILE *file, void *buf, size_t len, int flags)
{
    Socket *socket = GetSocket(file);

    if (!socket)
        return -1;

    return socket->Recv(buf, len, flags);
}

int SockFS::Sendto(FILE *file, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    Socket *socket = GetSocket(file);

    if (!socket)
        return -1;

    return socket->Sendto(buf, len, flags, dest_addr, addrlen);
}

int SockFS::Shutdown(FILE *file, int how)
{
    Socket *socket = GetSocket(file);

    if (!socket)
        return -1;

    return socket->Shutdown(how);
}

Socket *SockFS::GetSocket(FILE *file)
{
    int ino = file->dentry->inode->ino;
    return SocketManager::GetSocket(ino);
}
