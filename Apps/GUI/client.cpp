#include "client.h"
#include <AndyOS.h>
#include <sys/msg.h>
#include "GUI/messages.h"
#include "string.h"

static bool initialized = 0;
static bool connected = false;
const int server_id = 1;

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

int Client::CreateWindow(char* title)
{
    BOOL_RESPONSE response;
    if (SendRequest(CREATE_WINDOW_REQUEST(title), response))
    {
        
    }
}

template <class IN, class OUT>
bool Client::SendRequest(IN req, OUT& res, bool ignore_disconnected)
{
    if (!connected && !ignore_disconnected)
        return false;

    MESSAGE response = send_message(server_id, GUI_MESSAGE_TYPE, &req, sizeof(IN));

    if (response.size != sizeof(OUT))
        return false;

    res = *(OUT*)response.data;
}