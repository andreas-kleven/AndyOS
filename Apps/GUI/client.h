#pragma once
#include <sys/msg.h>
#include "GUI/messages.h"

namespace gui
{
    class Client
    {
    public:
        static bool connected;

        static void Init();

        template <class IN, class OUT>
        static bool SendRequest(IN req, OUT& res, bool ignore_disconnected = false)
        {
            if (!connected && !ignore_disconnected)
                return false;

            MESSAGE response = send_message(1, GUI_MESSAGE_TYPE, &req, sizeof(IN));

            if (response.size != sizeof(OUT))
                return false;

            res = *(OUT*)response.data;
            return true;
        }

        template <class IN>
        static bool SendRequest(IN req, bool ignore_disconnected = false)
        {
            if (!connected && !ignore_disconnected)
                return false;

            post_message(1, GUI_MESSAGE_TYPE, &req, sizeof(IN));
            return true;
        }
    };
}