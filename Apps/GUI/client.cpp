#include "client.h"
#include <AndyOS.h>
#include "string.h"

namespace gui
{
    static bool initialized = 0;
    bool Client::connected = false;

    void Client::Init()
    {   
        if (initialized)
            return;

        char* a = new char;
        initialized = true;

        BOOL_RESPONSE response;
        if (SendRequest(SIMPLE_REQUEST(REQUEST_TYPE_CONNECT), response, true))
        {
            connected = response.success;
            debug_print("Connected: %i\n", connected);
        }
    }
}