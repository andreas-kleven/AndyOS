#pragma once
#include "GUI/messages.h"
#include "window.h"
#include <sys/socket.h>

namespace gui {
namespace client {

bool connected = false;

bool SendRequest(int type, int id, const void *in, void *out, int in_size, int out_size);
void MessageHandler(const MESSAGE &msg);
void AddWindow(Window *wnd);
void *Loop(void *arg);
void Init();

template <class IN, class OUT>
bool SendRequest(int type, int id, const IN &req, OUT &res, bool ignore_disconnected = false)
{
    if (!connected && !ignore_disconnected)
        return false;

    return SendRequest(type, id, &req, &res, sizeof(IN), sizeof(OUT));
}

template <class IN>
bool SendRequest(int type, int id, const IN &req, bool ignore_disconnected = false)
{
    if (!connected && !ignore_disconnected)
        return false;

    return SendRequest(type, id, &req, 0, sizeof(IN), 0);
}

}; // namespace client
} // namespace gui
