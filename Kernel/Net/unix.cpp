#include "unix.h"
#include "socketmanager.h"
#include "errno.h"

namespace Unix
{
    Socket *Accept(Socket *server)
    {
        server->accept_event.Set();
        server->connect_event.Wait();
        return server->tmp_client;
    }

    int Connect(Socket *client, Socket *server)
    {
        if (!server->listening)
            return -ECONNREFUSED;

        server->connect_mutex.Aquire();
        server->accept_event.Wait();

        Socket *socket = SocketManager::CreateSocket();
        socket->Init(server->domain, server->type, server->protocol);
        
        socket->unix_socket = client;
        client->unix_socket = socket;
        server->tmp_client = socket;

        server->connect_event.Set();
        server->connect_mutex.Release();

        return 0;
    }

    int Listen(Socket *server)
    {
        server->accept_event = Event(false, true);
        server->connect_event = Event(false, true);
        server->listening = true;
        return 0;
    }

    int Send(Socket *socket, const void *buf, size_t len, int flags)
    {
        if (!socket->unix_socket)
            return -1;

        socket->unix_socket->HandleData(buf, len);
        return len;
    }

} // namespace Unix
