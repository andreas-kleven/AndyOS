#include "client.h"
#include <AndyOS.h>
#include "string.h"

namespace gui
{
    using namespace messages;

    static bool initialized = 0;
    bool Client::connected = false;

    List<Window*> windows;

    void Client::Init()
    {   
        if (initialized)
            return;

        initialized = true;

        set_message(MessageHandler);
        
        BOOL_RESPONSE response;
        if (SendRequest(SIMPLE_REQUEST(REQUEST_TYPE_CONNECT), response, true))
        {
            connected = response.success;
            debug_print("Connected: %i\n", connected);
        }
    }

    void Client::AddWindow(Window* wnd)
    {
        windows.Add(wnd);
    }

    MESSAGE Client::MessageHandler(MESSAGE msg)
    {
        if (msg.type == GUI_MESSAGE_TYPE)
        {
            if (msg.size < 4)
                return MESSAGE(0);

            for (int i = 0; i < windows.Count(); i++)
            {
                Window* wnd = windows[i];
                WINDOW_MESSAGE* wnd_msg = (WINDOW_MESSAGE*)msg.data;

                if (wnd->id == wnd_msg->id)
                {
                    wnd->HandleMessage(msg);
                    break;
                }
            }
        }

        return MESSAGE(0);
    }
}