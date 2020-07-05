#pragma once
#include "socket.h"

namespace Unix
{
    Socket *Accept(Socket *server);
    int Connect(Socket *client, Socket *server);
    int Listen(Socket *server);
    int Send(Socket *socket, const void *buf, size_t len, int flags);
} // namespace Unix
