#include <Net/socketmanager.h>
#include <net.h>
#include <sync.h>
#include <list.h>
#include <FS/vfs.h>

namespace SocketManager
{
    Mutex mutex;
    List<Socket *> sockets;
    int next_port = 32768;
    int next_id = 1;

    Socket *CreateSocket()
    {
        mutex.Aquire();
        int id = next_id++;

        Socket *socket = new Socket(id);
        sockets.Add(socket);

        mutex.Release();
        return socket;
    }

    int Shutdown(Socket *socket, int how)
    {
        if (!socket)
            return -1;

        socket->Shutdown(how);

        //sockets.Remove(...);
        return -1;
    }

    int AllocPort()
    {
        return next_port++;
    }

    Socket *GetSocket(int id)
    {
        mutex.Aquire();

        for (int i = 0; i < sockets.Count(); i++)
        {
            Socket *socket = sockets[i];

            if (socket->id == id)
            {
                mutex.Release();
                return socket;
            }
        }

        mutex.Release();
        return 0;
    }

    Socket *GetSocket(int domain, int type, int protocol, sockaddr *addr)
    {
        mutex.Aquire();

        for (int i = 0; i < sockets.Count(); i++)
        {
            Socket *socket = sockets[i];

            if (socket->domain != domain || socket->type != type || socket->protocol != protocol)
                continue;

            if (socket->domain == AF_INET)
            {
                sockaddr_in *inet_addr = (sockaddr_in *)addr;
                sockaddr_in *sock_addr = (sockaddr_in *)socket->addr;

                if ((sock_addr && sock_addr->sin_port == inet_addr->sin_port) || (!sock_addr && htons(socket->src_port) == inet_addr->sin_port))
                {
                    mutex.Release();
                    return socket;
                }
            }
        }

        mutex.Release();
        return 0;
    } // namespace SocketManager

    Socket *GetUdpSocket(unsigned short port)
    {
        sockaddr_in addr;
        addr.sin_port = htons(port);
        return GetSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, (sockaddr *)&addr);
    }

    Socket *GetUnixSocket(const sockaddr_un *addr)
    {
        DENTRY *dentry = VFS::GetDentry(addr->sun_path);

        if (!dentry)
            return 0;

        int id = dentry->inode->ino;
        return GetSocket(id);
    }
} // namespace SocketManager
