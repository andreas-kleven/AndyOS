#include <FS/sockfs.h>
#include <FS/vfs.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>
#include <Net/socketmanager.h>
#include <debug.h>

int SockFS::Mount(BlockDriver *driver)
{
    VFS::AllocInode(root_dentry);
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
    return Send(file, buf, size, 0);
}

int SockFS::Seek(FILE *file, long offset, int origin)
{
    return -1;
}

int SockFS::GetChildren(DENTRY *parent, const char *find_name)
{
    return -1;
}

int SockFS::Create(int domain, int type, int protocol, DENTRY *&dentry)
{
    Socket *socket = SocketManager::CreateSocket();

    int ret = 0;

    if ((ret = socket->Init(domain, type, protocol)))
    {
        //TODO: SocketManager::RemoveSocket(socket);
        return ret;
    }

    dentry = CreateSocketDentry(socket->id);
    return 0;
}

int SockFS::Accept(FILE *file, sockaddr *addr, socklen_t addrlen, int flags, DENTRY *&dentry)
{
    Socket *socket = GetSocket(file);

    if (!socket)
        return -1;

    int id = socket->Accept(addr, addrlen, flags);

    if (id < 0)
        return id;

    dentry = CreateSocketDentry(id);
    return 0;
}

int SockFS::Bind(FILE *file, const struct sockaddr *addr, socklen_t addrlen)
{
    Socket *socket = GetSocket(file);
    DENTRY *parent;
    int ret = 0;

    if (!socket)
        return -1;

    sockaddr_un *unix_addr = (sockaddr_un *)addr;
    const char *filename;
    const char *parentname;

    if (addr->sa_family == AF_UNIX && unix_addr->sun_path[0])
    {
        char *copy = strdup(unix_addr->sun_path);
        filename = basename(copy);
        parentname = dirname(copy);
        delete[] copy;

        parent = VFS::GetDentry(parentname);

        if (!parent)
            return -ENOENT;
    }

    if ((ret = socket->Bind(addr, addrlen)))
        return ret;

    if (addr->sa_family == AF_UNIX)
    {
        if (parent)
        {
            DENTRY *dentry = VFS::AllocDentry(parent, filename);
            dentry->inode = file->dentry->inode; // TODO: Symlink?
            VFS::AddDentry(parent, dentry);
        }
    }

    return 0;
}

int SockFS::Connect(FILE *file, const sockaddr *addr, socklen_t addrlen)
{
    Socket *socket = GetSocket(file);

    if (!socket)
        return -1;

    return socket->Connect(addr, addrlen);
}

int SockFS::Listen(FILE *file, int backlog)
{
    Socket *socket = GetSocket(file);

    if (!socket)
        return -1;

    return socket->Listen(backlog);
}

int SockFS::Recv(FILE *file, void *buf, size_t len, int flags)
{
    Socket *socket = GetSocket(file);

    if (!socket)
        return -1;

    return socket->Recv(buf, len, flags);
}

int SockFS::Recvfrom(FILE *file, void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t addrlen)
{
    Socket *socket = GetSocket(file);

    if (!socket)
        return -1;

    return socket->Recvfrom(buf, len, flags, src_addr, addrlen);
}

int SockFS::Send(FILE *file, const void *buf, size_t len, int flags)
{
    Socket *socket = GetSocket(file);

    if (!socket)
        return -1;

    return socket->Send(buf, len, flags);
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

DENTRY *SockFS::CreateSocketDentry(int socket_id)
{
    DENTRY *dentry = VFS::AllocDentry(0, 0);
    VFS::AllocInode(dentry);
    dentry->inode->mode = S_IFSOCK;
    dentry->inode->ino = socket_id;
    VFS::AddDentry(root_dentry, dentry);
    return dentry;
}
