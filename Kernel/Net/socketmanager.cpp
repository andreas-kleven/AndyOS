#include "socketmanager.h"
#include "net.h"
#include "sync.h"
#include "list.h"

namespace SocketManager
{
    Mutex mutex;
    List<Socket> sockets;
    int next_port = 32768;
    int next_id = 1;

    Socket *CreateSocket(int domain, int type, int protocol)
    {
        mutex.Aquire();
        int id = next_id++;
        sockets.Add(Socket(id, domain, type, protocol));
        Socket *socket = &sockets.Last();
        mutex.Release();
        return socket;
    }

    int Shutdown(Socket *socket, int how)
    {
        if (!socket)
            return -1;

        socket->Shutdown(how);

        //sockets.Remove(...);
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
            Socket *socket = &sockets[i];

            if (socket->id == id)
            {
                mutex.Release();
                return socket;
            }
        }

        mutex.Release();
    }

    Socket *GetSocket(int domain, int type, int protocol, sockaddr *addr)
    {
        mutex.Aquire();

        for (int i = 0; i < sockets.Count(); i++)
        {
            Socket *socket = &sockets[i];

            if (socket->domain != domain || socket->type != type || socket->protocol != protocol)
                continue;

            if (socket->domain == AF_INET)
            {
                sockaddr_in *inet_addr = (sockaddr_in *)addr;
                sockaddr_in *sock_addr = (sockaddr_in *)&socket->addr;

                if (sock_addr->sin_port == inet_addr->sin_port)
                {
                    mutex.Release();
                    return socket;
                }
            }
        }

        mutex.Release();
        return 0;
    }

    Socket *GetUdpSocket(unsigned short port)
    {
        sockaddr_in addr;
        addr.sin_port = htons(port);
        return GetSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, (sockaddr *)&addr);
    }
} // namespace SocketManager
